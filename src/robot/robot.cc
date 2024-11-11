#include "robot/robot.hpp"

namespace robo {
void Robot::run(std::atomic<bool>& running) {
    bindVirtualDev();

#ifdef USE_WEBOTS
    for (auto &ctrl: ctrls) {
        ctrl->runner.bind(webots_io);
    }
#endif
    for (auto &io: ios) {
        io->run();
    }

    devInit();

    for (auto &ctrl: ctrls) {
        ctrl->runner.run();
    }

#ifdef USE_WEBOTS
    while (running && webots_io.step() != -1);
#else
    while (running);
#endif

    for (auto &ctrl: ctrls) {
        ctrl->runner.stop();
    }
    for (auto &io: ios) {
        io->stop();
    }
}

}

