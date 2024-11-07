#pragma once

#include <toml++/toml.hpp>

#include "ctrl/ctrl.hpp"
#include "util/util.hpp"

using namespace robo::util;
namespace robo {
namespace ctrl {
template <typename T>
class WheelLeggedChassis: public Ctrl{
public:
    explicit WheelLeggedChassis(const toml::table &config, const std::string &name = "wheel legged chassis"):
        Ctrl(name, config > Key<int>("cycle_ms")),
        leg{T(config > Key<float, T::link_param_num>("leg_param"), "left"),
            T(config > Key<float, T::link_param_num>("leg_param"), "right")} {
    }
    ~WheelLeggedChassis() override = default;

    T leg[2];

    void ctrlLoop() override {
    }

private:

};
}
}


