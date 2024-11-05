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

    float l;
    float theta_l;

    void bind(const MotorBinder &binder1, const MotorBinder &binder2);
    void update();
    std::array<float, 2> getPosition();
    std::array<float, 2> getSpeed();
    void setForce(float F_n, float tau_j);

private:
    const float l_a;
    const float l_b;
    const float l_c;
    const std::string name {"virtual"};

    Motor motor1 {"motor1 in five_link(" + name + ")"};
    Motor motor2 {"motor2 in five_link(" + name + ")"};

    float varphi_1;
    float varphi_2;
    float varphi_B_1;
    float x_B_2;
    float y_B_2;
    float x_C;
    float y_C;
    Eigen::Matrix<float, 2, 2> jacobian_trans;

    void forward_solve();
    void jacobian_calc();
};
}
}

