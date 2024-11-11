#include "ctrl/runner.hpp"
#include "util/util.hpp"

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
        LOG(ERROR) << "[Runner<" + name + ">] You haven't bound webots_io before running!";
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

void Runner::bind(io::Webots &webots_io_) {
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
    util::setThisThreadRealTime(cycle_ms * util::Ms<util::Ns>);

    timespec t_set;
    clock_gettime(CLOCK_MONOTONIC, &t_set);
    while (running) {
        
        timespec t_start;
        clock_gettime(CLOCK_MONOTONIC, &t_start);

        task();

        timespec t_now;
        clock_gettime(CLOCK_MONOTONIC, &t_now);
        util::incTime(t_set, cycle_ms * util::Ms<util::Ns>);
        long t_remain = util::getTimeDuration(t_now, t_set);
        if (t_remain <= 0) {
            long t_task = util::getTimeDuration(t_start, t_now);
            long t_total = cycle_ms * util::Ms<util::Ns> - t_remain;
            util::incTime(t_set, t_total);
            LOG(ERROR) << "[Runner<" + name + ">] Task execution time exceeded cycle time (" 
                       << "task: " << t_task << "ns, " << "total: "
                       << t_total << "/" << cycle_ms * util::Ms<util::Ns> << "ns)";
        }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t_set, nullptr);
    }
}
#endif
} // namespace run
} // namespace robo

