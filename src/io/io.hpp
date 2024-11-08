#pragma once

#include <cstring>
#include <functional>
#include <vector>
#include <string>
#include <thread>
#include <map>
#include <easylogging++.h>

#include "util/util.hpp"

namespace robo {
namespace io {
class Io {
public:
    explicit Io(const std::string &name, const int buffer_size):
        name(name),
        buffer_size(buffer_size),
        buffer(new uint8_t[buffer_size]) {
    }
    virtual ~Io() {
        delete[] buffer;
        if (thread && thread->joinable()) {
            thread->join();
        }
    }

    void run() {
        if (thread != nullptr) {
            LOG(ERROR) << "[IO<" + name + ">] Repeated run an IO!";
            return;
        }
        running = true;
        thread = new std::thread([this]() { thread_func(); });
        LOG(INFO) << "[IO<" + name + ">] Start running...";
    }
    void stop() {
        running = false;
        if (thread != nullptr) {
            thread->join();
            LOG(INFO) << "[IO<" + name + ">] Stopped!";
        }
    }

    const std::string name;

protected:
    const int buffer_size;
    uint8_t *buffer;
    std::atomic<bool> running {false};

    virtual void thread_func() = 0;

private:
    std::thread *thread {nullptr};

};

template <typename Tkey>
class IoKey: public Io {
public:
    explicit IoKey(const std::string &name, const int buffer_size):
        Io(name, buffer_size) {
    }
    ~IoKey() override = default;

    using key_type = Tkey;

    std::map<Tkey, std::function<void (const uint8_t *, const int len)>> unpackers;

    virtual int read(Tkey &key, uint8_t *data) = 0;

private:
    void thread_func() override {
        while (running) {
            int len;
            Tkey key;
            len = read(key, buffer);
            if (len <= 0) {
                LOG(WARNING) << "[IO<" + name + ">] Read Error!";
            } else {
                auto it = unpackers.find(key);
                if (it == unpackers.end()) {
                    if constexpr (robo::util::is_streamable<Tkey>::value) {
                        LOG(WARNING) << "[IO<" + name + ">] Unbound key read: " << key << ".";
                    } else {
                        LOG(WARNING) << "[IO<" + name + ">] Unbound key read.";
                    }
                } else {
                    it->second(buffer, len);
                }
            }
        }
    }
};

class IoNoKey: public Io {
public:
    explicit IoNoKey(const std::string &name, const int buffer_size):
        Io(name, buffer_size) {
    }
    ~IoNoKey() override = default;

    std::vector<std::function<bool (const uint8_t *, const int len)>> unpackers;

    virtual int read(uint8_t *data) = 0;
    virtual void readDirtyHook() = 0;

private:
    void thread_func() override {
        while (running) {
            memset(buffer, 0, buffer_size);
            int len = read(buffer);
            if (len <= 0) {
                LOG(WARNING) << "[IO<" + name + ">] Read Error!";
            } else {
                bool is_unpacked = false;
                for (auto &unpacker: unpackers) {
                    is_unpacked = unpacker(buffer, len);
                    if (is_unpacked) {
                        break;
                    }
                }
                if (!is_unpacked) {
                    LOG(WARNING) << "[IO<" + name + ">] The data read does not have a matching unpacker.";
                    readDirtyHook();
                }
            }
        }
    }
};
}
}

