#pragma once

#include "src/ctrl/wheel_legged_chassis/wheel_legged_chassis.hpp"
#include "src/dev/unitree_motor.hpp"
#include "src/robot/robot.hpp"
#include "src/virtual/five_link.hpp"

namespace robo {
class Aokee: public Robot {
public:
    explicit Aokee(const toml::table &config);
    ~Aokee() override = default;

private:
    robo::ctrl::WheelLeggedChassis<vir::FiveLink> chassis;

    robo::io::Serial rs485;
    robo::dev::UnitreeMotor motor[6];
    
    void bindVirtualDev() override;
    void devInit() override;
};
}

