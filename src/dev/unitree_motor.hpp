#pragma once

#include "io/serial.hpp"
#include "virtual/motor.hpp"
#include "dev/dev.hpp"

namespace robo {
namespace dev {
class UnitreeMotor: public Dev<robo::io::Serial> {
public:
    explicit UnitreeMotor(robo::io::Serial &io_serial, const std::string &motor_name, const int id, const int dir);
    ~UnitreeMotor() override = default;

    robo::vir::MotorBinder binder {
        .setTorque = [this](float torque) { this->setTorque(torque); },
        .angle= &this->angle,
        .speed = &this->speed,
    };

    void setTorque(float torque);
    void setAngelOffset(float angle_offset_) {
    }

private:
    const int id;
    const int dir;
    uint8_t send_msg[17] {0xFD, 0xEE, (uint8_t)((id << 4) + (1 << 3))};
    float speed {0.0f};
    float angle {0.0f};
    float angle_offset {0.0f};
    int time_last {0};

    virtual bool unpack(const char *data, const int len) override;
};
}
}
