#pragma once

#include <atomic>
#include <string>
#include <type_traits>

#include "item.hpp"
#include "param.hpp"

namespace show {
extern item::Graphs graphs;

void task(std::atomic<bool> &running, const std::string &cfg_);

/*show::plot(a);*/
/*show::plot(a, b, c, ...);*/
/*show::plot(a, {"b", b}, c, ...);*/
/*show::plot("graph_name", a, {"b", b}, c, ...);*/

using NameValue = std::pair<std::string, float>;

template <typename T>
void add_to_vector(std::vector<NameValue> &params, T &arg) {
    if constexpr (std::is_same_v<T, float>) {
        params.emplace_back(param::DEFAULT_VAR_NAME_PREFIX, arg);
    } else if constexpr (std::is_same_v<T, NameValue>) {
        params.emplace_back(arg);
    }
}

template <typename ... Args>
decltype(auto) to_vector(Args &... args) {
    std::vector<NameValue> params;
    (..., add_to_vector(params, args));
    return params;
}

template <typename ... Args>
void plotGraph(item::Graph &graph, Args &... args) {
    auto t = item::getTime();
    auto vars = to_vector(args ...);
    int i = 0;
    for (auto &var: vars) {
        if (var.first == param::DEFAULT_VAR_NAME_PREFIX) {
            var.first += std::to_string(i);
        }
        if (i == vars.size()) {
            graph.emplace_back(var.first);
        }
        graph[i].addPoint(t, var.second);
        i++;
    }
}

template <typename ... Args>
void plot(const std::string &graph_name, Args &... args) {
    graphs.try_emplace(graph_name, item::Graph());
    plot_graph(graphs[graph_name], args...);
}

template <typename... Args>
void plot(Args... args) {
    plot(param::DEFAULT_GRAPH_NAME, args...);
}



}

