#pragma once

#include "io/serial.hpp"
#include "virtual/motor.hpp"
#include "dev/dev.hpp"

namespace robo {
namespace dev {
class UnitreeMotor: public Dev<robo::io::Serial> {
public:
    explicit UnitreeMotor(robo::io::Serial &io_serial, const std::string &motor_name, const uint8_t id, const int dir);
    ~UnitreeMotor() override = default;

    robo::vir::MotorBinder binder {
        .setTorque = [this](float torque) { this->setTorque(torque); },
        .angle= &this->angle,
        .speed = &this->speed,
        .actual = this,
    };

    void setTorque(float torque);
    void setAngelOffset(float angle_offset_);
    void setAngel(float current_angle);

private:
    enum Mode {
        DEFALUT           = 0,
        FOC               = 1,
        ENCODER_CALIBRATE = 2,
    };
    enum Error {
        NORMAL            = 0,
        OVER_HEAT         = 1,
        OVER_CURRENT      = 2,
        OVER_VOLTAGE      = 3,
        ENCODER_FAULT     = 4,
        BUS_UNDER_VOLTAGE = 5,
        WINDING_OVER_HEAT = 6,
        RESERVED          = 7,
    };
    static const std::array<std::string, 8> err_msg;

    const uint8_t id;
    const int dir;

    static constexpr uint8_t SEND_HEAD[2] {0xfe, 0xee};
    static constexpr uint8_t RESV_HEAD[2] {0xfd, 0xee};
    static constexpr float RATIO {6.33f};
    static constexpr int SEND_TIME_US {200};

    struct Info {
        uint8_t id: 4;
        Mode mode: 3  = FOC;
        uint8_t reserved: 1;
    } __attribute__((packed));
    struct Command {
        int16_t torque_set;
        int16_t speed_set;
        int32_t angle_set;
        int16_t k_pos_set;
        int16_t k_spd_set;
    } __attribute__((packed));
    struct Feedback {
        int16_t torque_fbk;
        int16_t speed_fbk;
        int32_t angle_fbk;
        int8_t temp_fbk;
        Error err: 3 = NORMAL;
        uint16_t force_fbk: 12;
        uint8_t reserved: 1;
    } __attribute__((packed));

    template <typename T>
    struct Data {
        uint8_t head[2];
        Info info;
        T msg;
        uint16_t CRC;
    } __attribute__((packed));

    Data<Command> send_data;
    float speed {0.0f};
    float angle {0.0f};
    float angle_offset {0.0f};
    int8_t temp {0};
    Error err {NORMAL};

    virtual bool unpack(const uint8_t *data, const int len) override;
};
}
}
