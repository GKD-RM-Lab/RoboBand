#include <string_view>
#include <stdexcept>

#include "robot/robot.hpp"
#include "util/util.hpp"

namespace robo {
void Robot::run(std::atomic<bool>& running) {
#ifdef USE_WEBOTS
    for (auto ctrl: ctrls) {
        ctrl->runner.bind(webots_io);
    }
#endif
    for (auto io: ios) {
        io->run();
    }
    for (auto ctrl: ctrls) {
        ctrl->runner.run();
    }

#ifdef USE_WEBOTS
    while (running && webots_io.step() != -1);
#else
    while (running);
#endif

    for (auto io: ios) {
        io->stop();
    }
    for (auto ctrl: ctrls) {
        ctrl->runner.stop();
    }
}

}

