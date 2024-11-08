#include <Aokee/Aokee.hpp>
#include "src/util/util.hpp"

using namespace robo::util;
namespace robo {
Aokee::Aokee(const toml::table &config):
    chassis(config > "ctrl" > "chassis"),
    rs485(config > "io" > Key<std::string>("rs485"), 16, 4000000, 100),
    motor {
        robo::dev::UnitreeMotor(rs485, "0", 0, 1),
        robo::dev::UnitreeMotor(rs485, "1", 1, -1),
        robo::dev::UnitreeMotor(rs485, "2", 2, 1),
        robo::dev::UnitreeMotor(rs485, "3", 3, -1),
        robo::dev::UnitreeMotor(rs485, "4", 4, 1),
        robo::dev::UnitreeMotor(rs485, "5", 5, -1),
    } {
    ctrls.push_back(&chassis);
    ios.push_back(&rs485);
    bindVirtualDev();
    devInit();
}

void Aokee::bindVirtualDev() {
    chassis.wheel[0].bind(motor[0].binder);
    chassis.leg[0].motor1.bind(motor[2].binder);
    chassis.leg[0].motor2.bind(motor[3].binder);
    chassis.wheel[1].bind(motor[1].binder);
    chassis.leg[1].motor1.bind(motor[4].binder);
    chassis.leg[1].motor2.bind(motor[5].binder);
}

void Aokee::devInit() {
}
}

