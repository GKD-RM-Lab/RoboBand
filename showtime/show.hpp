#pragma once

#include <atomic>
#include <string>

namespace show {
void task(std::atomic<bool> &running, const std::string &cfg);
void plot();
}

