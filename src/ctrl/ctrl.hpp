#pragma once

#include <string>

#include "ctrl/runner.hpp"

namespace robo {
namespace ctrl {
class Ctrl {
public:
    explicit Ctrl(const std::string &name, const float cycle_ms):
        runner(name, [this]() { ctrlLoop(); }, cycle_ms) {
    }
    virtual ~Ctrl() = default;

    robo::run::Runner runner;

    virtual void ctrlLoop() = 0;

};
}
}
