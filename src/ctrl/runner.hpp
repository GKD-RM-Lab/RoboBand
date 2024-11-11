#pragma once

#include <thread>
#include <atomic>
#include <functional>
#include <pthread.h>

#ifdef USE_WEBOTS
#include "io/webots.hpp"
#endif

namespace robo {
namespace run {
class Runner {
public:
    explicit Runner(const std::string &name, std::function<void ()> task, const float cycle_ms):
        name(name),
        cycle_ms(cycle_ms),
        task(task) {
    }
    ~Runner() {
        if (thread && thread->joinable()) {
            thread->join();
        }
    }

    void run();
    void stop();
#ifdef USE_WEBOTS
    void bind(robo::io::Webots &webots_io_);
#endif

private:
    const std::string name;
    const float cycle_ms;
    std::atomic<bool> running {false};
    std::function<void ()> task;
    std::thread *thread {nullptr};
#ifdef USE_WEBOTS
    robo::io::Webots *webots_io;
#endif

    void thread_func();

};
}
}
