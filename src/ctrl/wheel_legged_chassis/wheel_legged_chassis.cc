#include "ctrl/wheel_legged_chassis/wheel_legged_chassis.hpp"
#include <util/util.hpp>

using namespace robo::util;
namespace robo {
namespace ctrl {
WheelLeggedChassis::WheelLeggedChassis(const toml::table &config, const std::string &name):
    Ctrl(name, getValue<int>(config, "cycle_ms")) {
    
}

void WheelLeggedChassis::ctrlLoop() {
    motor.setTorque(0.2f);
}


}
}
