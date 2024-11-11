#ifdef USE_WEBOTS
#pragma once

#include "src/robot/robot.hpp"
#include "src/ctrl/balance_sys/balance_sys.hpp"
#include "src/dev/webots_imu.hpp"
#include "src/dev/webots_motor.hpp"

namespace robo {
class LqrSim: public Robot {
public:
    explicit LqrSim(const toml::table &config);
    ~LqrSim() override = default;

private:
    ctrl::BalanceSys balance_sys;

    dev::WebotsImu imu;
    dev::WebotsMotor wheel_motor[2];
    dev::WebotsMotor joint_motor[2];
    
    void bindVirtualDev() override;
    void devInit() override;
};
}

#endif

