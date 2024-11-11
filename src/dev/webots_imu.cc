#ifdef USE_WEBOTS

#include "dev/webots_imu.hpp"

namespace robo {
namespace dev {
WebotsImu::WebotsImu(io::Webots &webots_io, const std::string &inertial_name, const std::string &gyro_name):
    Dev(webots_io),
    inertial(*webots_io.robot.getInertialUnit(inertial_name)),
    gyro(*webots_io.robot.getGyro(gyro_name)) {
    inertial.enable(webots_io.robot.getBasicTimeStep());
    gyro.enable(webots_io.robot.getBasicTimeStep());
}

void WebotsImu::update() {
    webots_io.inertialGetRollPitchYaw(inertial, angle);
    webots_io.gyroGetValues(gyro, speed);
}
}
}

#endif
