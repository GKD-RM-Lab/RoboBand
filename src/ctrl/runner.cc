#include <easylogging++.h>
#include <ctime>
#include <cerrno>
#include <chrono>

#include "ctrl/runner.hpp"

namespace robo {
namespace run {
void Runner::stop() {
    running = false;
    if (thread != nullptr) {
        thread->join();
        LOG(INFO) << "[Runner<" + name + ">] Stopped!";
    }
}

#ifdef USE_WEBOTS
void Runner::run() {
    if (webots_io == nullptr) {
        LOG(ERROR) << "[Runner<" + name + ">] You haven't bound robo::io::webots before running!";
        return;
    }
    if (thread != nullptr) {
        LOG(ERROR) << "[Runner<" + name + ">] Repeated run a Runner!";
        return;
    }

    running = true;
    thread = new std::thread([this]() { thread_func(); });
    LOG(INFO) << "[Runner<" + name + ">] Start running...";
}

void Runner::thread_func() {
    int time_set = webots_io->time;
    while (running) {
        time_set += cycle_ms;
        if (webots_io != nullptr) {
            if (webots_io->sync_point != nullptr) {
                webots_io->sync_point->arrive_and_wait();
            }
            if (time_set >= webots_io->time) {
                task();
            }
        }
    }
}

void Runner::bind(robo::io::Webots &webots_io_) {
    webots_io = &webots_io_;
    webots_io->bind_runner_num++;
}

#else
void Runner::run() {
    if (thread != nullptr) {
        LOG(ERROR) << "[Runner<" + name + ">] Repeated run a Runner!";
    } else {
        running = true;
        thread = new std::thread([this]() { thread_func(); });
        LOG(INFO) << "[Runner<" + name + ">] Start running...";
    }
}

void Runner::thread_func() {
    auto next_time = std::chrono::high_resolution_clock::now();
    while (running) {
        next_time += std::chrono::milliseconds(cycle_ms);
        auto sleep_time = next_time - std::chrono::high_resolution_clock::now();
        timespec ts = {
            .tv_sec = duration_cast<seconds>(sleep_time).count(),
            .tv_nsec = duration_cast<nanoseconds>(sleep_duration).count() % 1000000000,
        };
        nanosleep(&ts, nullptr);

        task();
    }
}
#endif
} // namespace run
} // namespace robo
