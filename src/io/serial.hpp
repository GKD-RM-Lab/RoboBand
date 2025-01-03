#pragma once

#include <termios.h>
#include <csignal>
#include <poll.h>
#include <map>

#include "io/io.hpp"

namespace robo {
namespace io {
class Serial: public IoNoKey {
public:
    explicit Serial(const std::string &name, const int buffer_size, const speed_t Bbaudrate, const int timeout_inline_us, const int timeout_wait_us);
    ~Serial() override;

    void addRxHeadTail(const std::string &head, const std::string &tail);
    int read(uint8_t *data) override;
    bool send(const uint8_t *message, const int len, const int sleep_us = -1);

private:
    const speed_t Bbaudrate;

    std::map<std::string, std::string> head_tails;

    pollfd pfd;
    sigset_t sigmask;
    timespec timeout_inline {0};
    timespec timeout_wait {0};

    long baudrate(speed_t Bbaudrate) {
        switch (Bbaudrate) {
            case B0: return 0;
            case B50: return 50;
            case B75: return 75;
            case B110: return 110;
            case B134: return 134;
            case B150: return 150;
            case B200: return 200;
            case B300: return 300;
            case B600: return 600;
            case B1200: return 1200;
            case B1800: return 1800;
            case B2400: return 2400;
            case B4800: return 4800;
            case B9600: return 9600;
            case B19200: return 19200;
            case B38400: return 38400;
            case B57600: return 57600;
            case B115200: return 115200;
            case B230400: return 230400;
            case B460800: return 460800;
            case B500000: return 500000;
            case B576000: return 576000;
            case B921600: return 921600;
            case B1000000: return 1000000;
            case B1152000: return 1152000;
            case B1500000: return 1500000;
            case B2000000: return 2000000;
            case B2500000: return 2500000;
            case B3000000: return 3000000;
            case B3500000: return 3500000;
            case B4000000: return 4000000;
            default: return -1;
        }
    }
};
}
}
