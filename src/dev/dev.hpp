#pragma once

#include <type_traits>

#include "ext/easyloggingpp/src/easylogging++.h"
#include "io/io.hpp"
#ifdef USE_WEBOTS
#include "io/webots.hpp"
#endif

namespace robo {
namespace dev {
template <typename IO, typename Enable = void>
class Dev;

template <typename IO>
class Dev<IO, typename std::enable_if_t<std::is_base_of_v<io::IoKey<typename IO::key_type>, IO>>> {
public:
    explicit Dev(const std::string &name, IO &io, const typename IO::key_type io_key):
        name(name),
        io(io),
        io_key(io_key) {
        auto it = io.unpackers.find(io_key);
        if (it != io.unpackers.end()) {
            if constexpr (util::is_streamable<typename IO::key_type>::value) {
                LOG(ERROR) << "[Dev<" + name + R"(>] You used duplicate key ")" << io_key << R"(" when binging to IO ")" << io.name << R"("!)";
            } else {
                LOG(ERROR) << "[Dev<" + name + R"(>] You used duplicate key when binging to IO ")" << io.name << R"("!)";
            }
        } else {
            io.unpackers.emplace(io_key, [this](const uint8_t *data, const int len) -> bool { return unpack(data, len); });
        }
    }
    Dev(const Dev &) = delete;
    virtual ~Dev() = default;
    

    const typename IO::key_type getIoKey() {
        return io_key;
    }

    const std::string name;

protected:
    IO &io;
    const typename IO::key_type io_key;

private:
    virtual bool unpack(const uint8_t *data, const int len) = 0;
};

template <typename IO>
class Dev<IO, typename std::enable_if_t<std::is_base_of_v<io::IoNoKey, IO>>> {
public:
    explicit Dev(const std::string &name, IO &io):
        name(name),
        io(io) {
        io.unpackers.push_back([this](const uint8_t *data, const int len) -> bool { return unpack(data, len); });
    }
    Dev(const Dev &) = delete;
    virtual ~Dev() = default;

    const std::string name;

protected:
    IO &io;

private:
    virtual bool unpack(const uint8_t *data, const int len) = 0;
};

#ifdef USE_WEBOTS
template <>
class Dev<io::Webots> {
public:
    explicit Dev(io::Webots &webots_io):
        webots_io(webots_io) {
        webots_io.updaters.push_back([this]() { update(); });
    }
    Dev(const Dev &) = delete;
    virtual ~Dev() = default;

protected:
    io::Webots &webots_io;

private:
    virtual void update() = 0;

};
#endif
}
}
