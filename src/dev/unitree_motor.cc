#include <numbers>
#include <string>

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
    io_serial.addRxHeadTail(std::string((char *)RESV_HEAD, sizeof(RESV_HEAD)), "");
    send_data.head[0] = SEND_HEAD[0];
    send_data.head[1] = SEND_HEAD[1];
    send_data.info.id = id;
}

void UnitreeMotor::setTorque(float torque) {
    if (err != NORMAL) {
        torque = 0.0f;
    }

    if (id == 3) {
        LOG(INFO) << speed << ", " << torque;
    }

    send_data.msg.torque_set = (int16_t)(util::abs_limit(torque * dir, 127.9f) * 256.0f);
    send_data.CRC = CRC::Calculate(&send_data, sizeof(send_data) - sizeof(send_data.CRC), CRC::CRC_16_KERMIT());

    io.send((uint8_t *)&send_data, sizeof(send_data), SEND_TIME_US);
}

void UnitreeMotor::setAngelOffset(float angle_offset_) {
    angle_offset = angle_offset_;
}

void UnitreeMotor::setAngel(float current_angle) {
    angle_offset = angle - current_angle;
}

bool UnitreeMotor::unpack(const uint8_t *data, const int len) {
    auto fbk = (Data<Feedback> *)data;

    if (fbk->head[0] != RESV_HEAD[0] || fbk->head[1] != RESV_HEAD[1] || fbk->info.id != id) {
        return false;
    }
    if (fbk->CRC != CRC::Calculate(data, sizeof(*fbk) - sizeof(fbk->CRC), CRC::CRC_16_KERMIT())) {
        return false;
    }

    angle = fbk->msg.angle_fbk / 32768.0f * 2.0f * std::numbers::pi_v<float> / RATIO * dir - angle_offset;
    speed = fbk->msg.speed_fbk / 256.0f * 2.0f * std::numbers::pi_v<float> / RATIO * dir;
    temp = fbk->msg.temp_fbk;

    err = fbk->msg.err;
    if (err != NORMAL) {
        LOG(ERROR) << "[UnitreeMotor<" + name + ">] Error(" << err << "): " + err_msg[err];        
    }

    return true;
}
}
}
