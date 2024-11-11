#include <Aokee/Aokee.hpp>

#include "dev/unitree_motor.hpp"
#include "src/util/util.hpp"

using namespace robo::util;
namespace robo {
Aokee::Aokee(const toml::table &config):
    chassis(config > "ctrl" > "chassis"),
    rs485(config > "io" > Key<std::string>("rs485"), 16, B4000000, 10, 100000),
    motor {
        dev::UnitreeMotor(rs485, "0", 0, (config > Key<float, 6>("motor_dir"))[0]),
        dev::UnitreeMotor(rs485, "1", 1, (config > Key<float, 6>("motor_dir"))[1]),
        dev::UnitreeMotor(rs485, "2", 2, (config > Key<float, 6>("motor_dir"))[2]),
        dev::UnitreeMotor(rs485, "3", 3, (config > Key<float, 6>("motor_dir"))[3]),
        dev::UnitreeMotor(rs485, "4", 4, (config > Key<float, 6>("motor_dir"))[4]),
        dev::UnitreeMotor(rs485, "5", 5, (config > Key<float, 6>("motor_dir"))[5]),
    },
    pid_retract {
        util::Pid(config > Key<float, 5>("pid_retract")),
        util::Pid(config > Key<float, 5>("pid_retract")),
        util::Pid(config > Key<float, 5>("pid_retract")),
        util::Pid(config > Key<float, 5>("pid_retract")),
    },
    RETRACT_SPEED(config > Key<float>("retract_speed")),
    MOTOR_STOP_SPEED(config > Key<float>("motor_stop_speed")),
    RETRACT_CTRL_CYCLE_MS(config > Key<float>("retract_ctrl_cycle_ms")),
    RETRACT_TIME_MIN_MS(config > Key<float>("retract_time_min_ms")),
    JOINT_LIMIT_ANGLE(config > Key<float>("joint_limit_angle")) {
    ctrls.push_back(&chassis);
    ios.push_back(&rs485);
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
    int t = 0;

    vir::Motor *joint_motor[] = {
        &chassis.leg[0].motor1,
        &chassis.leg[0].motor2,
        &chassis.leg[1].motor1,
        &chassis.leg[1].motor2,
    };

    while (true) {
        for (std::size_t i = 0; i < sizeof(joint_motor) / sizeof(vir::Motor *); i++) {
            (RETRACT_SPEED - joint_motor[i]->getSpeed()) >> pid_retract[i] >> *joint_motor[i];
        }
        util::sleep(RETRACT_CTRL_CYCLE_MS * util::Ms<util::Ns>);
        
        if (t > RETRACT_TIME_MIN_MS) {
            bool stopped = true;
            for (auto m: joint_motor) {
                if (util::abs(m->getSpeed()) > MOTOR_STOP_SPEED)
                    stopped = false;
            }
            if (stopped) {
                for (auto m: joint_motor) {
                    m->actual<dev::UnitreeMotor>()->setAngel(JOINT_LIMIT_ANGLE);
                }
                break;
            }
        }
        /*std::cout << std::setprecision(3)*/
        /*          << chassis.leg[0].motor1.getAngle() << ", "*/
        /*          << chassis.leg[0].motor2.getAngle() << ", "*/
        /*          << chassis.leg[1].motor1.getAngle() << ", "*/
        /*          << chassis.leg[1].motor2.getAngle() << std::endl;*/

        t += RETRACT_CTRL_CYCLE_MS;
    }
}
}

