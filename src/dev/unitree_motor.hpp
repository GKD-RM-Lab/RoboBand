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
    enum Error {
        NORMAL = 0,
        OVER_HEAT = 1,
        OVER_CURRENT = 2,
        OVER_VOLTAGE = 3,
        ENCODER_FAULT = 4,
        BUS_UNDER_VOLTAGE = 5,
        WINDING_OVER_HEAT = 6,
        RESERVED = 7,
    };
    static const std::array<std::string, 8> err_msg;

    const int id;
    const int dir;
    uint8_t send_msg[17] {0xFD, 0xEE, (uint8_t)((id << 4) + (1 << 3))};
    float speed {0.0f};
    float angle {0.0f};
    float angle_offset {0.0f};
    int8_t temp {0};
    Error err {NORMAL};

    virtual bool unpack(const uint8_t *data, const int len) override;
};
}
}
