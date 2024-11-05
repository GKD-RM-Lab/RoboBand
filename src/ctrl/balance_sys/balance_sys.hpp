#pragma once

#include <toml++/toml.hpp>
#include <eigen3/Eigen/Eigen>

#include "ctrl/ctrl.hpp"
#include "virtual/motor.hpp"
#include "virtual/imu.hpp"

namespace robo {
namespace ctrl {
class BalanceSys: public Ctrl{
public:
    explicit BalanceSys(const std::string &name, const toml::table &config);
    ~BalanceSys() override = default;

    robo::vir::Imu imu;
    robo::vir::Motor wheel_motor[2];
    robo::vir::Motor joint_motor[2];

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
