#include <Aokee/Aokee.hpp>
#include "src/util/util.hpp"

using namespace robo::util;
namespace robo {
Aokee::Aokee(const toml::table &config):
    chassis(config > "ctrl" > "chassis"),
    rs485(config > "io" > Key<std::string>("rs485"), 16, 4000000, 100),
    motor(rs485, "0", 0, 1) {
    ctrls.push_back(&chassis);
    bindVirtualDev();
    devInit();
}

void Aokee::bindVirtualDev() {
    chassis.leg[0].motor1.bind(motor.binder);
    chassis.leg[0].motor2.bind(motor.binder);
    chassis.leg[1].motor1.bind(motor.binder);
    chassis.leg[1].motor2.bind(motor.binder);
}

void Aokee::devInit() {

}
}

