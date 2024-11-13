#include <ctime>

#include "item.hpp"
#include "param.hpp"

namespace show {
namespace item {
int BUFFER_SIZE {30000};

Var::Var(const std::string &name):
    name(name) {
    data.reserve(BUFFER_SIZE);
    addPoint(0.0f, 0.0f);
}

void Var::addPoint(float x, float y) {
    if (data.size() < BUFFER_SIZE)
        data.push_back(ImVec2(x,y));
    else {
        data[offset] = ImVec2(x,y);
        offset =  (offset + 1) % BUFFER_SIZE;
    }
}

void Var::erase() {
    if (data.size() > 0) {
        data.shrink(0);
        offset  = 0;
    }
}

double getTime() {
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1000000000.0f;
}
}
}

