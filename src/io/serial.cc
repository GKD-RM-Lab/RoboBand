#include <exception>

#include "io/serial.hpp"

namespace robo {
namespace io {
Serial::Serial(const std::string &name, const int buffer_size, const int baudrate, const int timeout_ms, const std::string &eol):
    IoNoKey(name, buffer_size),
    eol(eol),
    serial(name, baudrate, serial::Timeout(1, timeout_ms, 0, timeout_ms, 0)),
    baudrate(baudrate),
    timeout_ms(timeout_ms) {
    try {
        if (!serial.isOpen()) {
            LOG(ERROR) << "[IO<" + name + ">] Disconnected!";
            return;
        }
    } catch (const std::exception &err) {
        LOG(ERROR) << "[IO<" + name + ">] An exception was thrown: " << err.what();
        return;
    }
}

bool Serial::isConnected() {
    try {
        if (serial.isOpen()) {
            return true;
        } else {
            LOG(ERROR) << "[IO<" + name + ">] Disconnected!";
            serial.open();
            return false;
        }
    } catch (const std::exception &err) {
        LOG(ERROR) << "[IO<" + name + ">] An exception was thrown: " << err.what();
        return false;
    }
}

int Serial::read(uint8_t *data) {
    try {
        int n;
        if (eol != "") {
            std::string str = serial.readline(buffer_size, eol);
            n = str.length();
            memcpy(data, str.c_str(), n);
        } else {
            n = serial.read((uint8_t *)data, buffer_size);
            if (n != buffer_size) {
                n = -1;
            }
        }
        return n;
    } catch (const std::exception &err) {
        LOG(ERROR) << "[IO<" + name + ">] An exception was thrown: " << err.what();
        return -1;
    }
}

bool Serial::send(const uint8_t *message, const int len) {
    try {
        int n = serial.write((const uint8_t *)message, len);
        if (n == len) {
            return true;
        } else {
            return false;
        }
    } catch (const std::exception &err) {
        LOG(ERROR) << "[IO<" + name + ">] An exception was thrown: " << err.what();
        return false;
    }
}
}
}
