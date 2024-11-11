#include <easylogging++.h>
#include <fstream>
#include <csignal>
#include <filesystem>

#include "robot_create.hpp"

INITIALIZE_EASYLOGGINGPP

std::atomic<bool> running {true};
std::string project_path;

void SigintHandler(int signum) {
    LOG(INFO) << "Interrupt signal (" << signum << ") received!";
    running = false;
    exit(0);
}

int main(int argc, char **argv) {
#ifndef USE_WEBOTS
    signal(SIGINT, SigintHandler);
#endif
    project_path = std::filesystem::absolute(argv[0]).parent_path().parent_path().parent_path();

    std::string robot_name;
    std::string user_config_path = project_path + "/user.toml";

    el::Loggers::configureFromGlobal((project_path + "/log/log.conf").c_str());
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    std::remove("myeasylog.log");

    LOG(DEBUG) << "Program start. ------------------------------------------";

    if (argc > 2) {
        LOG(ERROR) << "You passed the wrong command line arguments!";
        return 1;
    } else if (argc == 2) {
        user_config_path = argv[1];
    }

    std::ifstream tomlFile(user_config_path);
    if (!tomlFile.is_open()) {
        LOG(ERROR) << R"(Could not open file ")" + user_config_path + R"("!)";
        return 1;
    }

    auto robot = robo::robotCreate(user_config_path);

    if (robot == nullptr) {
        return 1;
    }

    robot->run(running);

    delete robot;

    LOG(INFO) << "Main thread exiting...";
}

