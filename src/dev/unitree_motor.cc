#include <numbers>

#include "dev/unitree_motor.hpp"
#include "CRC.h"

namespace robo {
namespace dev {
const std::array<std::string, 8> UnitreeMotor::err_msg {
    "Normal",
    "Overheat",
    "Overcurrent",
    "Overvoltage",
    "Encoder fault",
    "Bus undervoltage",
    "Winding overheat",
    "Reserved",
};

UnitreeMotor::UnitreeMotor(robo::io::Serial &io_serial, const std::string &motor_name, const uint8_t id, const int dir):
    Dev(motor_name, io_serial),
    id(id),
    dir(dir) {
    send_data.info.id = id;
}

void UnitreeMotor::setTorque(float torque) {
    if (err != NORMAL) {
        torque = 0.0f;
    }

    send_data.msg.torque_set = (int16_t)(util::abs_limit(torque, 127.9f) * 256.0f);
    send_data.CRC = CRC::CalculateBits(&send_data, sizeof(send_data) - sizeof(send_data.CRC), CRC::CRC_16_KERMIT());

    io.send((uint8_t *)&send_data, sizeof(send_data));
}

void UnitreeMotor::setAngelOffset(float angle_offset_) {
    angle_offset = dir * angle_offset_;
}

bool UnitreeMotor::unpack(const uint8_t *data, const int len) {
    auto fbk = (Data<Feedback> *)data;

    if (fbk->head != HEAD || fbk->info.id != id) {
        return false;
    }

    if (fbk->CRC != CRC::CalculateBits(data, sizeof(*fbk) - sizeof(fbk->CRC), CRC::CRC_16_KERMIT())) {
        return false;
    }

    angle = fbk->msg.angle_fbk / 32768.0f * 2.0f * std::numbers::pi_v<float>;
    speed = fbk->msg.speed_fbk / 256.0f * 2.0f * std::numbers::pi_v<float>;
    temp = fbk->msg.temp_fbk;

    err = fbk->msg.err;
    if (err != NORMAL) {
        LOG(ERROR) << "[UnitreeMotor<" + name + ">] Error(" << std::to_string(err) << "): " + err_msg[err];        
    }

    return true;
}
}
}
