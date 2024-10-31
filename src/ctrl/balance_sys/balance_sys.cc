#include "ctrl/balance_sys/balance_sys.hpp"
#include "util/util.hpp"
// DEBUG
#include "io/socket.hpp"

inline float abs_limit(float x, const float lim) {
    if (x < -lim) {
        x = -lim;
    }
    if (x > lim) {
        x = lim;
    }
    return x;
}

using namespace robo::util;
namespace robo {
namespace ctrl {
BalanceSys::BalanceSys(const toml::table &config, const std::string &name):
    Ctrl(name, getValue<int>(config, "cycle_ms")),
    cycle_ms(getValue<int>(config, "cycle_ms")),
    K(Eigen::Map<Eigen::Matrix<float, 10, 4>>(getArray<float, 4 * 10>(config, "K").data()).transpose()),
    radius_wheel(getValue<float>(config, "radius_wheel")) {
}

void BalanceSys::ctrlLoop() {

    float theta_b = imu.getAngle(robo::vir::Imu::Pitch);
    float dot_theta_b = imu.getSpeed(robo::vir::Imu::Pitch);
    float theta_j_1 = joint_motor[0].getAngle();
    float theta_j_2 = joint_motor[1].getAngle();
    float dot_theta_j_1 = joint_motor[0].getSpeed();
    float dot_theta_j_2 = joint_motor[1].getSpeed();
    float theta_l_1 = theta_b + theta_j_1;
    float theta_l_2 = theta_b + theta_j_2;
    float dot_theta_l_1 = dot_theta_b + dot_theta_j_1;
    float dot_theta_l_2 = dot_theta_b + dot_theta_j_2;

    // TODO s int & lim
    state_set << 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    state_ref << (wheel_motor[0].getAngle() + wheel_motor[1].getAngle()) * radius_wheel / 2,
                 (wheel_motor[0].getSpeed() + wheel_motor[1].getSpeed()) * radius_wheel / 2,
                 imu.getAngle(robo::vir::Imu::Yaw),
                 imu.getSpeed(robo::vir::Imu::Yaw),
                 theta_l_1,
                 dot_theta_l_1,
                 theta_l_2,
                 dot_theta_l_2,
                 theta_b,
                 dot_theta_b;

    // TODO rad_format phi
    ctrl_vec = K * (state_set - state_ref); 

    float ctrl_val[4] = {
        abs_limit(ctrl_vec(0), 6),
        abs_limit(ctrl_vec(1), 6),
        abs_limit(ctrl_vec(2), 20),
        abs_limit(ctrl_vec(3), 20),
    };

    wheel_motor[0].setTorque(ctrl_val[0]);
    wheel_motor[1].setTorque(ctrl_val[1]);
    joint_motor[0].setTorque(ctrl_val[2]);
    joint_motor[1].setTorque(ctrl_val[3]);

    static io::Socket plot("plot", 14515, 100, 100);
    static auto ip = util::to_in_addr("localhost");
    // DEBUG
    struct Data {
        int32_t id;
        float t;
        float d[9];
    } __attribute__((packed));
    /*Data d = { 1, 0, { state_ref(0), state_ref(2), state_ref(4), state_ref(6), state_ref(8), ctrl_vec(0), ctrl_vec(1), ctrl_vec(2), ctrl_vec(3) } };*/
    Data d = { 1, 0, { state_ref(0) * K(0, 0), state_ref(2) * K(0, 2), state_ref(4) * K(0, 4), state_ref(6) * K(0, 6), state_ref(8) * K(0, 8), ctrl_vec(0), ctrl_vec(1), ctrl_vec(2), ctrl_vec(3) } };
    /*Data d = { 1, 0, { state_ref(0) * K(2, 0), state_ref(2) * K(2, 2), state_ref(4) * K(2, 4), state_ref(6) * K(2, 6), state_ref(8) * K(2, 8), ctrl_vec(0), ctrl_vec(1), ctrl_vec(2), ctrl_vec(3) } };*/
    plot.send(std::make_tuple(ip, 14514), (char *)&d, sizeof(d));
    LOG(INFO) << "state: " << state_ref(0) << "," << state_ref(2) << "," << state_ref(4) << "," << state_ref(6) << "," << state_ref(8) << ", tau: " << ctrl_vec(0) << "," << ctrl_vec(1) << "," << ctrl_vec(2) << "," << ctrl_vec(3);
    /*LOG(INFO) << "state dot: " << state_ref(1) << "," << state_ref(3) << "," << state_ref(5) << "," << state_ref(7) << "," << state_ref(9) << ", tau: " << ctrl_vec(0) << "," << ctrl_vec(1) << "," << ctrl_vec(2) << "," << ctrl_vec(3);*/
    LOG(INFO) << "contribute: " << state_ref(0) * K(2, 0) << "," << state_ref(2) * K(2, 2) << "," << state_ref(4) * K(2, 4) << "," << state_ref(6) * K(2, 6) << "," << state_ref(8) * K(2, 8);
    /*LOG(INFO) << "contribute dot: " << state_ref(1) * K(2, 1) << "," << state_ref(3) * K(2, 3) << "," << state_ref(5) * K(2, 5) << "," << state_ref(7) * K(2, 7) << "," << state_ref(9) * K(2, 9);*/
}
}
}
