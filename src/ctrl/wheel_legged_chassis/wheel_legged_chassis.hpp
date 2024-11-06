#pragma once

#include <toml++/toml.hpp>

#include "ctrl/ctrl.hpp"
#include "virtual/motor.hpp"

namespace robo {
namespace ctrl {
class WheelLeggedChassis: public Ctrl{
public:
    explicit WheelLeggedChassis(const toml::table &config, const std::string &name = "wheel legged chassis");
    ~WheelLeggedChassis() override = default;

    vir::Motor motor;

    void ctrlLoop() override;

private:

};
}
}


