#include <fstream>
#include <csignal>
#include <filesystem>

#include "ext/easyloggingpp/src/easylogging++.h"
#include "robot_create.hpp"

INITIALIZE_EASYLOGGINGPP

std::atomic<bool> running {true};
std::string project_path;

void SigintHandler(int signum) {
    LOG(INFO) << "Interrupt signal (" << signum << ") received!";
    running = false;
#warning
    exit(0);
}

int main(int argc, char **argv) {
#ifndef USE_WEBOTS
    signal(SIGINT, SigintHandler);
#endif
    project_path = std::filesystem::absolute(argv[0]).parent_path().parent_path().parent_path();

    std::string robot_name;
    std::string user_config_path = project_path + "/cfg/robot.toml";

    /*init logger*/
    el::Loggers::configureFromGlobal((project_path + "/cfg/log.conf").c_str());
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    std::remove("myeasylog.log");

    LOG(DEBUG) << "Program start. ------------------------------------------";

    /*optional: pass your configuration file from command line arguments*/
    if (argc > 2) {
        LOG(ERROR) << "You passed the wrong command line arguments!";
        return -1;
    } else if (argc == 2) {
        user_config_path = argv[1];
    }

    /*open your configuration file*/
    std::ifstream tomlFile(user_config_path);
    if (!tomlFile.is_open()) {
        LOG(ERROR) << R"(Could not open file ")" + user_config_path + R"("!)";
        LOG(ERROR) << "Error " << errno << " from open: " << std::strerror(errno);
        return errno;
    }

    /*create a robot based on your configuration file*/
    auto robot = robo::robotCreate(user_config_path);
    if (robot == nullptr) {
        return -1;
    }

    /*run the robot until the program gets a SIGINT signal*/
    robot->run(running);

    delete robot;

    LOG(INFO) << "Main thread exiting...";
}

