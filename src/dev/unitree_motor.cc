#include "dev/unitree_motor.hpp"
#include "CRC.h"

namespace robo {
namespace dev {
UnitreeMotor::UnitreeMotor(robo::io::Serial &io_serial, const std::string &motor_name, const int id, const int dir):
    Dev(motor_name, io_serial),
    id(id),
    dir(dir) {
}

void UnitreeMotor::setTorque(float torque) {
    int16_t tau = (int16_t)(util::abs_limit(torque, 127.9f) * 256.0f);
    memcpy(&send_msg[3], &tau, 2);

    uint16_t crc = CRC::CalculateBits(send_msg, 15, CRC::CRC_16_KERMIT());
    memcpy(&send_msg[15], &crc, 2);

    io.send((char *)send_msg, 17);
}
}
}
