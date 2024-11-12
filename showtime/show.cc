#include "show.hpp"
#include "ext/tomlplusplus/include/toml++/toml.hpp"

namespace show {
static bool need_show = false;
static bool window_closed = false;

static toml::table cfg;

void init(const std::string& cfg_path) {
    try {
        cfg = toml::parse_file(cfg_path);
        /*plot = new plot::Plot(*socket, cfg);*/
    } catch (const toml::parse_error &err) {
        throw ;
    }
}

void stop() {
}

void task() {
}

void plot() {
}
}

