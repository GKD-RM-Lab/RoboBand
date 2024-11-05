#pragma once
#ifdef USE_WEBOTS

#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/PositionSensor.hpp>

#include "virtual/motor.hpp"
#include "dev/dev.hpp"

namespace robo {
namespace dev {
class WebotsMotor: public Dev<robo::io::Webots> {
public:
    explicit WebotsMotor(robo::io::Webots &webots_io, const std::string &motor_name, const int dir);
    ~WebotsMotor() override = default;

    robo::vir::MotorBinder binder {
        .setTorque = [this](float torque) { this->setTorque(torque); },
        .angle= &this->angle,
        .speed = &this->speed,
    };

    void setTorque(float torque) {
        webots_io.motorSetTorque(motor, dir * torque);
    }
    void setAngelOffset(float angle_offset_) {
        angle_offset = dir * angle_offset_;
    }

private:
    webots::Motor &motor;
    webots::PositionSensor &encoder;

    const int dir;
    float speed {0.0f};
    float angle {0.0f};
    float angle_last {0.0f};
    float angle_offset {0.0f};
    int time_last {0};

    virtual void update() override;

};
}
}

#endif

