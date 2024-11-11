#pragma once

#include <string>
#include <atomic>
#include <vector>
#include <toml++/toml.hpp>

#include "ctrl/ctrl.hpp"
#include "io/io.hpp"
#ifdef USE_WEBOTS
#include "io/webots.hpp"
#endif

namespace robo {
class Robot {
public:
    explicit Robot() = default;
    virtual ~Robot() = default;

    void run(std::atomic<bool>& running);

protected:
    std::vector<ctrl::Ctrl *> ctrls;
#ifdef USE_WEBOTS
    io::Webots webots_io;
#endif
    std::vector<io::Io *> ios;

    virtual void bindVirtualDev() = 0;
    virtual void devInit() = 0;

};
}

