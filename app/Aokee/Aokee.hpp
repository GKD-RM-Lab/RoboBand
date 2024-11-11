#pragma once

#include "src/ctrl/wheel_legged_chassis/wheel_legged_chassis.hpp"
#include "src/dev/unitree_motor.hpp"
#include "src/robot/robot.hpp"
#include "src/virtual/five_link.hpp"
#include "util/siso.hpp"

namespace robo {
class Aokee: public Robot {
public:
    explicit Aokee(const toml::table &config);
    ~Aokee() override = default;

private:
    ctrl::WheelLeggedChassis<vir::FiveLink> chassis;

    io::Serial rs485;
    dev::UnitreeMotor motor[6];

    util::Pid pid_retract[4];
    const float RETRACT_SPEED;
    const float MOTOR_STOP_SPEED;
    const float RETRACT_CTRL_CYCLE_MS;
    const float RETRACT_TIME_MIN_MS;
    const float JOINT_LIMIT_ANGLE;
    
    void bindVirtualDev() override;
    void devInit() override;
};
}

