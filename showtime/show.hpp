#pragma once

#include <atomic>
#include <string>
#include <type_traits>

#include "item.hpp"
#include "param.hpp"

namespace show {
extern item::Graphs graphs;
extern bool need_show;

void task(std::atomic<bool> &running, const std::string &cfg_ = "");

/*show::plot(a);*/
/*show::plot(a, b, c, ...);*/
/*show::plot(a, std::pair("b", b), c, ...);*/
/*show::plot("graph_name", a, std::pair("b", b), c, ...);*/

using NameValue = std::pair<std::string, float>;

template <typename T>
void add_to_vector(std::vector<NameValue> &params, T &&arg) {
    if constexpr (std::is_convertible_v<T, float>) {
        params.emplace_back(DEFAULT_VAR_NAME_PREFIX, arg);
    } else if constexpr (std::is_convertible_v<T, NameValue>) {
        params.emplace_back(arg);
    } else {
        static_assert(true, "Error: T is not a vaild type");
    }
}

template <typename ... Args>
decltype(auto) to_vector(Args &&... args) {
    std::vector<NameValue> params;
    (..., add_to_vector(params, std::forward<Args>(args)));
    return params;
}

template <typename ... Args>
void plotGraph(item::Graph &graph, Args &&... args) {
    auto t = item::getTime();
    auto vars = to_vector(std::forward<Args>(args) ...);
    std::size_t i = 0;
    for (auto &var: vars) {
        if (var.first == DEFAULT_VAR_NAME_PREFIX) {
            var.first += std::to_string(i);
        }
        if (i == graph.size()) {
            graph.emplace_back(var.first);
        }
        graph[i].addPoint(t, var.second);
        i++;
    }
}

template <typename ... Args>
void plot(const std::string &graph_name, Args &&... args) {
    need_show = true;
    graphs.try_emplace(graph_name, item::Graph());
    plotGraph(graphs[graph_name], std::forward<Args>(args) ...);
}

template <typename T, typename ... Args>
auto plot(T &&arg, Args &&... args) -> std::enable_if_t<!std::is_convertible_v<T, std::string>> {
    plot(DEFAULT_GRAPH_NAME, arg, std::forward<Args>(args) ...);
}
}

