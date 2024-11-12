#include "robot/robot.hpp"
#include "src/util/util.hpp"

namespace robo {
void Robot::run(std::atomic<bool>& running) {
    /*bind actual devices to virtual devices*/
    bindVirtualDev();
#ifdef USE_WEBOTS
    for (auto &ctrl: ctrls) {
        ctrl->runner.bind(webots_io);
    }
#endif

    /*run ios*/
    for (auto &io: ios) {
        io->run();
    }

    /*init actual devices*/
    devInit();

    /*run controllers*/
    for (auto &ctrl: ctrls) {
        ctrl->runner.run();
    }

    /*keep running until `running` is set to false*/
#ifdef USE_WEBOTS
    while (running && webots_io.step() != -1) {
#else
    while (running) {
#endif
        util::sleep(MAIN_SLEEP_MS * util::Ms<util::Ns>);
    }

    /*stop ios & controllers*/
    for (auto &ctrl: ctrls) {
        ctrl->runner.stop();
    }
    for (auto &io: ios) {
        io->stop();
    }
}

}

