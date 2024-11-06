#include "robot_create.hpp"
#include "lqr_sim/lqr_sim.hpp"
#include "Aokee/Aokee.hpp"

extern std::string project_path;

namespace robo {
Robot *robotCreate(const std::string &user_config_path) {
    toml::table config;
    toml::table user_config;
    Robot *robot = nullptr;

    try {
        user_config = toml::parse_file(user_config_path);
    } catch (const toml::parse_error &err) {
        LOG(ERROR) << R"(Parsing your config ")" + user_config_path + R"(" failed!)";
        LOG(ERROR) << err;
        return nullptr;
    }

    if (!user_config.contains("robot")) {
        LOG(ERROR) << R"(Your config ")" + user_config_path + R"(" does not contain a key named "robot"!)";
        return nullptr;
    }

    auto robot_name = user_config["robot"].value_or(std::string_view("err"));

    try {
#ifdef USE_WEBOTS
        if (robot_name == "lqr_sim") {
            LOG(DEBUG) << "Robot name: " << robot_name;
            config = toml::parse_file(project_path + "/app/lqr_sim/default.toml");
            util::mergeConfig(user_config, config);
            robot = new LqrSim(config);
#else
       if (robot_name == "Aokee") {
            LOG(DEBUG) << "Robot name: " << robot_name;
            config = toml::parse_file(project_path + "/app/Aokee/default.toml");
            util::mergeConfig(user_config, config);
            robot = new Aokee(config);
#endif
        } else if (robot_name == "err") {
            LOG(ERROR) << R"(The value paired with the key "robot" in your config ")" + user_config_path + R"(" is not a string!)";
        } else {
            LOG(ERROR) << R"(The "robot" you specified in the config ")" + user_config_path + R"(" has no corresponding implementation!)";
        }
    } catch (const std::runtime_error &err) {
        LOG(ERROR) << "Robot initialization failed: " << err.what();
        return nullptr;
    }

    return robot;
}
}
