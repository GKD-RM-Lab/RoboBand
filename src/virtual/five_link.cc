#include "virtual/five_link.hpp"

namespace robo {
namespace vir {
void FiveLink::bind(const MotorBinder &binder1, const MotorBinder &binder2) {
    motor1.bind(binder1);
    motor2.bind(binder2);
}

void FiveLink::update() {
    varphi_1 = motor1.getAngle();
    varphi_2 = motor2.getAngle();

    forward_solve();
    jacobian_calc();

    Eigen::Vector<float, 2> joint;
    Eigen::Vector<float, 2> task;
    joint << varphi_1, varphi_2;
    task = jacobian_trans * joint;
    dot_l = task(0);
    dot_theta_l = task(1);
}

void FiveLink::setForce(float F_n, float tau_j) {
    Eigen::Vector<float, 2> force_virtual;
    Eigen::Vector<float, 2> force_actual;

    force_virtual << F_n, tau_j;
    force_actual = jacobian_trans * force_virtual;

    force_actual(0) >> motor1;
    force_actual(1) >> motor2;
}

void FiveLink::forward_solve() {
    float x_B_1 = l_a - l_b * cosf(varphi_1);
    x_B_2 = -l_a + l_b * cosf(varphi_2);
    float y_B_1 = l_b * sinf(varphi_1);
    y_B_2 = l_b * sinf(varphi_2);

    float x_B = x_B_1 - x_B_2;
    float y_B = y_B_1 - y_B_2;

    varphi_B_1 = acosf(sqrtf(x_B * x_B + y_B * y_B) / (2 * l_c)) - atan2f(y_B, x_B);

    x_C = l_a - l_b * cosf(varphi_1) - l_c * cosf(varphi_B_1); 
    y_C = l_b * sinf(varphi_1) + l_c * sinf(varphi_B_1);

    l = sqrtf(x_C * x_C + y_C * y_C);
    theta_l = atan2f(x_C, y_C);
}

void FiveLink::jacobian_calc() {
    float l_b_over_l = l_b / l;
    float varphi_B_2 = atan2f(y_C - y_B_2, x_C - x_B_2);
    float sin_varphi_B_1_plus_varphi_B_2 = sinf(varphi_B_1 + varphi_B_2);

    jacobian_trans << -l_b * cosf(theta_l + varphi_B_2) * sinf(varphi_1 - varphi_B_1) / sin_varphi_B_1_plus_varphi_B_2,
                      l_b_over_l * sinf(theta_l + varphi_B_2) * sinf(varphi_1 - varphi_B_1) / sin_varphi_B_1_plus_varphi_B_2,
                      -l_b * cosf(theta_l - varphi_B_1) * sinf(varphi_2 - varphi_B_2) / sin_varphi_B_1_plus_varphi_B_2,
                      l_b_over_l * sinf(theta_l - varphi_B_1) * sinf(varphi_2 - varphi_B_2) / sin_varphi_B_1_plus_varphi_B_2;
}
}
}

