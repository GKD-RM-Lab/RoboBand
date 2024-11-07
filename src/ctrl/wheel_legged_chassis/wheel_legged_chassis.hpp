#pragma once

#include <toml++/toml.hpp>
#include <eigen3/Eigen/Eigen>

#include "ctrl/ctrl.hpp"
#include "util/siso.hpp"
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
            T(config > Key<float, T::link_param_num>("leg_param"), "right")},
        F_GRAVITY(config > Key<float>("F_gravity")),
        pid_l(config > Key<float, 5>("pid_l_param")){
    }
    ~WheelLeggedChassis() override = default;

    T leg[2];

    void ctrlLoop() override {
        leg[0].update();
        leg[1].update();

        float F_phi, F_l, F_inertial;
        F_phi = 0.0f;
        F_inertial = 0.0f;

        // TODO not l but h
        float l_set = 0.25f;
        (l_set - 0.5f * (leg[0].l + leg[1].l)) >> pid_l >> F_l;
        
        Eigen::Vector<float, 4> F_ctrl {F_phi,
                                        F_l,
                                        F_GRAVITY,
                                        F_inertial};
        auto F_n = trans * F_ctrl;
        leg[0].setForce(F_n(0), 0.0f);
        leg[1].setForce(F_n(1), 0.0f);
    }

private:
    const float F_GRAVITY;
    Eigen::Matrix<float, 2, 4> trans {
        std::array{
            1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f
        }.data()};

    Pid pid_l;
};
}
}


