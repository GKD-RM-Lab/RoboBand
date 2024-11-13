#include "show.hpp"
#include "item.hpp"
#include "param.hpp"
#include "ui.hpp"
#include "ext/implot/implot.h"
#include "ext/imgui/backends/imgui_impl_glfw.h"
#include "ext/tomlplusplus/include/toml++/toml.hpp"

namespace show {
long TASK_WAIT_SLEEP_MS {100};
long TASK_SLEEP_MS {2};
float FONT_SIZE {18.0f};
std::string DEFAULT_GRAPH_NAME {"untitled"}; 
std::string DEFAULT_VAR_NAME_PREFIX {"var"}; 
int HEIGHT_OF_EACH {300};
float HISTORY_DEFAULT {5.0f};

item::Graphs graphs;
bool need_show {false};

static void show() {
    static bool pause = false;
    static bool title = true;
    static bool auto_fit = true;
    static bool link_axis_x = true;
    static ImPlotRange lims {0.0, 5.0};
    static float history_sync {HISTORY_DEFAULT};
    static float history_sync_max {HISTORY_DEFAULT * 5.0f};
    static constexpr ImPlotAxisFlags flags_x = ImPlotAxisFlags_NoInitialFit | ImPlotAxisFlags_NoSideSwitch | ImPlotAxisFlags_RangeFit;

    ImGui::SeparatorText("Options");

    bool pause_tmp = pause;
    ImGui::Checkbox("Title", &title);
    ImGui::SameLine();
    ImGui::Checkbox("Pause", &pause);
    ImGui::SameLine();
    ImGui::Checkbox("Auto Fit Y", &auto_fit);
    if (pause) {
        ImGui::SameLine();
        ImGui::Checkbox("Link Axis X", &link_axis_x);

        if (!pause_tmp) {
            double t = item::getTime();
            lims.Min = t - history_sync;
            lims.Max = t;
        }
    }

    ImGui::SliderFloat("History", &history_sync, 0.001, history_sync_max, "%.3f s");
    ImGui::SameLine();
    ImGui::InputFloat("max", &history_sync_max, 1.0f, 1.0f, "%.3f s");

    ImPlotAxisFlags flags_y = flags_x;
    if (auto_fit) {
        flags_y |= ImPlotAxisFlags_AutoFit;
    }

    ImGui::SeparatorText("Plots");
    if (!ImPlot::BeginSubplots("", graphs.size(), 1, ImVec2(-1, HEIGHT_OF_EACH * graphs.size()), pause ? 0 : ImPlotSubplotFlags_LinkAllX)) {
        return;
    }

    for (const auto &[name, graph]: graphs) {
        if (!ImPlot::BeginPlot(name.c_str(), ImVec2(-1,300), (title ? 0 : ImPlotFlags_NoTitle) | ImPlotFlags_Crosshairs)) {
            continue;
        }
        ImPlot::SetupAxes(nullptr, nullptr, flags_x, flags_y);

        auto t = item::getTime();
        if (!pause) {
                ImPlot::SetupAxisLimits(ImAxis_X1, t - history_sync, t, ImGuiCond_Always);
        } else {
            ImPlot::SetupAxisLinks(ImAxis_X1, link_axis_x ? &lims.Min : nullptr, link_axis_x ? &lims.Max : nullptr);
        }

        for (auto &var: graph) {
            ImPlot::PlotLine(var.name.c_str(),
                             &var.data[0].x,
                             &var.data[0].y,
                             var.data.size(),
                             0,
                             var.offset,
                             2 * sizeof(float));
        }
        ImPlot::EndPlot();
    }
    ImPlot::EndSubplots();
}

void task(std::atomic<bool> &running, const std::string &cfg_) {
    /*get config*/
    toml::table cfg;
    if (cfg_ != "") {
        try {
            cfg = toml::parse_file(cfg_);
            TASK_WAIT_SLEEP_MS = cfg["task_wait_sleep_ms"].value_or(TASK_WAIT_SLEEP_MS);
            TASK_SLEEP_MS = cfg["task_sleep_ms"].value_or(TASK_SLEEP_MS);
            FONT_SIZE = cfg["font_size"].value_or(FONT_SIZE);
            item::BUFFER_SIZE = cfg["buffer_size"].value_or(item::BUFFER_SIZE);
            DEFAULT_GRAPH_NAME = cfg["default_graph_name"].value_or(DEFAULT_GRAPH_NAME);
            DEFAULT_VAR_NAME_PREFIX = cfg["default_var_name_prefix"].value_or(DEFAULT_VAR_NAME_PREFIX);
            HEIGHT_OF_EACH = cfg["height_of_each"].value_or(HEIGHT_OF_EACH);
            HISTORY_DEFAULT = cfg["history_default"].value_or(HISTORY_DEFAULT);
        } catch (const toml::parse_error &err) {
            throw err;
        }
    }

    /*If not need show, keep waiting*/
    while (!need_show) {
        ImGui_ImplGlfw_Sleep(TASK_WAIT_SLEEP_MS);
        if (!running) {
            return;
        }
    }

    /*init*/
    GLFWwindow *window = ui::glfwWindowInit();
    ImGuiIO &io = ui::imguiInit(window, FONT_SIZE);

    /*run*/
    while (running && !glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        ui::loopStart(window, io);
        show();
        ui::loopEnd(window, io);
        ImGui_ImplGlfw_Sleep(TASK_SLEEP_MS);
    }

    ui::cleanup(window);
}
}

