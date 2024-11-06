#include <Aokee/Aokee.hpp>
#include "src/util/util.hpp"

using namespace robo::util;
namespace robo {
Aokee::Aokee(const toml::table &config):
    chassis(getTable(getTable(config, "ctrl"), "chassis")),
    rs485(getValue<std::string>(getTable(config, "io"), "rs485"), 16, 4000000, 100),
    motor(rs485, "0", 0, 1) {
    ctrls.push_back(&chassis);
    bindVirtualDev();
    devInit();
}

void Aokee::bindVirtualDev() {
    chassis.motor.bind(motor.binder);
}

void Aokee::devInit() {

}
}

