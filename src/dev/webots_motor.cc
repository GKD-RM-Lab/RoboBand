#ifdef USE_WEBOTS

#include "dev/webots_motor.hpp"

namespace robo {
namespace dev {
WebotsMotor::WebotsMotor(io::Webots &webots_io, const std::string& motor_name, const int dir):
    Dev(webots_io),
    motor(*webots_io.robot.getMotor(motor_name)),
    encoder(*motor.getPositionSensor()),
    dir(dir) {
    encoder.enable(webots_io.robot.getBasicTimeStep());
}

void WebotsMotor::update() {
    int time;
    float angle_tmp;
    webots_io.encoderGetValue(encoder, angle_tmp, time);
    if (time == time_last) {
        return;
    }
    angle = dir * angle_tmp - angle_offset;
    speed = (angle - angle_last) / (time - time_last);
    time_last = time;
    angle_last = angle;
}
}
}

#endif

