#pragma once

#include "io/io.hpp"
#include "serial/serial.h"

namespace robo {
namespace io {
class Serial: public IoNoKey {
public:
    // Receive data with a fixed length (buffer_size) or a specified packet tail (eol != ""), otherwise the real-time performance will be poor.
    // Usually use fixed-length data with a relatively large timeout, otherwise it may bring uncertain performance overhead.
    explicit Serial(const std::string &name, const int buffer_size, const int baudrate, const int timeout_ms, const std::string &eol = "");
    ~Serial() override = default;

    int read(char *data) override;
    bool send(const char *message, const int len);

private:
    const std::string eol;
    serial::Serial serial;
    int baudrate;
    int timeout_ms;

    bool isConnected();
};
}
}
