#include <fcntl.h>
#include <cerrno>
#include <termios.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <linux/serial.h>

#include "io/serial.hpp"
#include "util/util.hpp"

namespace robo {
namespace io {
Serial::Serial(const std::string &name, const int buffer_size, const speed_t Bbaudrate, const int timeout_inline_us, const int timeout_wait_us):
    IoNoKey(name, buffer_size),
    Bbaudrate(Bbaudrate) {

    sigemptyset(&sigmask);

    util::incTime(timeout_inline, timeout_inline_us * util::Us<util::Ns>);
    util::incTime(timeout_wait, timeout_wait_us * util::Us<util::Ns>);

    pfd.events = POLLIN;
    pfd.revents = 0;

    pfd.fd = ::open(name.c_str(), O_RDWR | O_NOCTTY);
    if (pfd.fd < 0) {
        LOG(ERROR) << "[Serial<" << name << ">] Error " << errno << " from open: " << std::strerror(errno);
        return;
    }
    if(::flock(pfd.fd, LOCK_EX | LOCK_NB) == -1) {
        LOG(ERROR) << "[Serial<" << name << ">] This port is already locked by another process.";
        return;
    }

    serial_struct serial;
    if (ioctl(pfd.fd, TIOCGSERIAL, &serial) != 0) {
        LOG(ERROR) << "[Serial<" << name << ">] Error " << errno << " from ioctl: " << std::strerror(errno);
        return;
    }
    serial.xmit_fifo_size = buffer_size * 1000;
    if (ioctl(pfd.fd, TIOCSSERIAL, &serial) != 0) {
        LOG(ERROR) << "[Serial<" << name << ">] Error " << errno << " from ioctl: " << std::strerror(errno);
        return;
    }

    struct termios tty;
    if(::tcgetattr(pfd.fd, &tty) != 0) {
        LOG(ERROR) << "[Serial<" << name << ">] Error " << errno << " from tcgetattr: " << std::strerror(errno);
        return;
    }

    tty.c_cflag &= (tcflag_t) ~(PARODD | PARENB | CSTOPB | CSIZE | CRTSCTS);
    tty.c_cflag |= (tcflag_t) (CS8 | CREAD | CLOCAL);

    tty.c_lflag &= (tcflag_t) ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOCTL | ISIG | IEXTEN);

    tty.c_iflag &= (tcflag_t) ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= (tcflag_t) ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IUCLC);

    tty.c_oflag &= (tcflag_t) ~(OPOST | ONLCR);

    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 0;

    ::cfsetspeed(&tty, Bbaudrate);

    if (::tcsetattr(pfd.fd, TCSANOW, &tty) != 0) {
        LOG(ERROR) << "[Serial<" << name << ">] Error " << errno << " from tcsetattr: " << std::strerror(errno);
        return;
    }
    if (::tcflush(pfd.fd, TCIOFLUSH) != 0) {
        LOG(ERROR) << "[Serial<" << name << ">] Error " << errno << " from tcflush: " << std::strerror(errno);
        return;
    }
}

Serial::~Serial() {
    if (::tcflush(pfd.fd, TCIOFLUSH) != 0) {
        LOG(ERROR) << "[Serial<" << name << ">] Error " << errno << " from tcflush: " << std::strerror(errno);
    }
    if(::close(pfd.fd) < 0) {
        LOG(ERROR) << "[Serial<" << name << ">] Error " << errno << " from close: " << std::strerror(errno);
    }
}

void Serial::addRxHeadTail(const std::string &head, const std::string &tail) {
    if (head.length() != 0)
        rx_heads.insert(head);
    if (tail.length() != 0)
        rx_tails.insert(tail);
}

int Serial::read(uint8_t *data) {
    auto read_len = 0;
    timespec t_start;
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    while (read_len < buffer_size) {
        auto n = ::read(pfd.fd, data + read_len, buffer_size - read_len);
        if (n < 0) {
            LOG(ERROR) << "[Serial<" << name << ">] Error " << errno << " from read: " << std::strerror(errno);
            return n;
        }

        if (n == 0) {
            ::ppoll(&pfd, 1, read_len == 0 ? &timeout_wait : &timeout_inline, &sigmask);
            if (!(pfd.revents & POLLIN)) {
                LOG(ERROR) << "[Serial<" << name << ">] Read timeout";
                break;
            }
        } else {
            // TODO head tail

            read_len += n;

            timespec t_now;
            clock_gettime(CLOCK_MONOTONIC, &t_now);
            if (util::getTimeDuration(t_start, t_now) <= 0) {
                break;
            }
        }
    }
    timespec t_now;
    clock_gettime(CLOCK_MONOTONIC, &t_now);
    std::cout << t_now.tv_nsec / util::Ms<util::Ns> << std::endl;
    return read_len;
}

void Serial::readDirtyHook() {
}
void Serial::readCompleteHook() {
}

bool Serial::send(const uint8_t *message, const int len, const int sleep_us) {
    // TODO lock
    auto ret = ::write(pfd.fd, message, len);
    if (ret < 0) {
        LOG(ERROR) << "[Serial<" << name << ">] Error " << errno << " from write: " << std::strerror(errno);
        return false;
    }

    util::sleep(sleep_us == -1 ? (long)(len) * util::Sec<util::Ns> / baudrate(Bbaudrate) : sleep_us * util::Us<util::Ns>);

    return true;
}
}
}
