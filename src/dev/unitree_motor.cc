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

UnitreeMotor::UnitreeMotor(robo::io::Serial &io_serial, const std::string &motor_name, const int id, const int dir):
    Dev(motor_name, io_serial),
    id(id),
    dir(dir) {
}

void UnitreeMotor::setTorque(float torque) {
    if (err != NORMAL) {
        torque = 0.0f;
    }

    int16_t tau = (int16_t)(util::abs_limit(torque, 127.9f) * 256.0f);
    memcpy(&send_msg[3], &tau, 2);

    uint16_t crc = CRC::CalculateBits(send_msg, 15, CRC::CRC_16_KERMIT());
    memcpy(&send_msg[15], &crc, 2);

    io.send((uint8_t *)send_msg, 17);
}

bool UnitreeMotor::unpack(const uint8_t *data, const int len) {
    if (data[0] != 0xFD || data[1] != 0xEE || (data[2] >> 4) != id) {
        return false;
    }

    if (data[15] == CRC::CalculateBits(data, 14, CRC::CRC_16_KERMIT())) {
        return false;
    }

    int32_t angle_fbk;
    int32_t speed_fbk;
    memcpy(&angle_fbk, &data[7], 4);
    angle = angle_fbk / 32768.0f * 2.0f * std::numbers::pi_v<float>;
    memcpy(&speed_fbk, &data[5], 2);
    speed = speed_fbk / 256.0f * 2.0f * std::numbers::pi_v<float>;
    memcpy(&temp, &data[11], 1);

    err = (Error)(data[12] >> 5);
    if (err != NORMAL) {
        LOG(ERROR) << "[UnitreeMotor<" + name + ">] Error(" << std::to_string(err) << "): " + err_msg[err];        
    }

    return true;
}
}
}
