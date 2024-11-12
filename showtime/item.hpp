#pragma once

#include <string>
#include <vector>
#include <map>

#include "ext/imgui/imgui.h"

namespace show {
namespace item {
class Var {
public:
    explicit Var(const std::string &name);
    ~Var() = default;

    const std::string name;
    ImVector<ImVec2> data;
    int offset = 0;

    void addPoint(float x, float y);
    void erase();
};

double getTime();

using Graph = std::vector<Var>;
using Graphs = std::map<std::string, Graph>;
}
}

