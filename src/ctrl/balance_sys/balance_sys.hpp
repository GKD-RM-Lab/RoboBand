#pragma once

#include "ext/tomlplusplus/include/toml++/toml.hpp"
#include "ext/eigen/Eigen/Eigen"
#include "ctrl/ctrl.hpp"
#include "virtual/motor.hpp"
#include "virtual/imu.hpp"

namespace robo {
namespace ctrl {
class BalanceSys: public Ctrl{
public:
    explicit BalanceSys(const toml::table &config, const std::string &name = "balance_sys");
    ~BalanceSys() override = default;

    vir::Imu imu;
    vir::Motor wheel_motor[2];
    vir::Motor joint_motor[2];

    void ctrlLoop() override;

private:
    const int CYCLE_MS;
    const float RADIUS_WHEEL;
    const float S_REF_MAX;
    const float PHI_REF_MAX;
    const float THETA_L_REF_MAX;
    const float THETA_B_REF_MAX;
    const float WHEEL_TORQUE_MAX;
    const float JOINT_TORQUE_MAX;
    Eigen::Matrix<float, 4, 10> K;

    Eigen::Vector<float, 10> state_ref;
    Eigen::Vector<float, 10> state_set;
    Eigen::Vector<float, 4> ctrl_vec;
};
}
}
