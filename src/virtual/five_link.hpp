#pragma once 

#include <string>
#include <virtual/motor.hpp>
#include <eigen3/Eigen/Eigen>

namespace robo {
namespace vir {
class FiveLink {
public:
    explicit FiveLink(const std::array<float, 3> &param,
                      const std::string &name = "virtual"):
        l_a(param[0]),
        l_b(param[1]),
        l_c(param[2]),
        name(name) {}
    ~FiveLink() = default;

    static constexpr std::size_t link_param_num = 3;
    const float l_a;
    const float l_b;
    const float l_c;
    const std::string name;

    float l {0.0f};
    float dot_l {0.0f};
    float theta_l {0.0f};
    float dot_theta_l {0.0f};

    Motor motor1 {"motor1 in five_link(" + name + ")"};
    Motor motor2 {"motor2 in five_link(" + name + ")"};

    void bind(const MotorBinder &binder1, const MotorBinder &binder2);
    void update();
    void setForce(float F_n, float tau_j);

private:
    float varphi_1 {0.0f};
    float varphi_2 {0.0f};
    float varphi_B_1 {0.0f};
    float x_B_2 {0.0f};
    float y_B_2 {0.0f};
    float x_C {0.0f};
    float y_C {0.0f};
    Eigen::Matrix<float, 2, 2> jacobian_trans;

    void forward_solve();
    void jacobian_calc();
};
}
}

