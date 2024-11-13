#include <fstream>
#include <csignal>
#include <filesystem>

#include "robot_create.hpp"
#include "ext/easyloggingpp/src/easylogging++.h"

#ifdef USE_SHOWTIME
#include "ext/showtime/src/show.hpp"
#endif

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
    std::string robot_config_path = project_path + "/cfg/robot.toml";

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
        robot_config_path = argv[1];
    }

    /*open your configuration file*/
    std::ifstream tomlFile(robot_config_path);
    if (!tomlFile.is_open()) {
        LOG(ERROR) << R"(Could not open file ")" + robot_config_path + R"("!)";
    }

    /*create a robot based on your configuration file*/
    auto robot = robo::robotCreate(robot_config_path);
    if (robot == nullptr) {
        return -1;
    }

    /*If you need to monitor variables, start the showtime thread*/
#ifdef USE_SHOWTIME
    auto showtime_thread = std::thread([&]() {
        try {
            show::task(running, project_path + "/" + toml::parse_file(robot_config_path)["showtime_config"].value_or(""));
        } catch (const std::exception &err) {
            LOG(ERROR) << "[ShowTime] Error: " << err.what();
            return;
        }
    });
#endif

    /*run the robot until the program gets a SIGINT signal*/
    robot->run(running);

    /*exit*/
    delete robot;
#ifdef USE_SHOWTIME
    showtime_thread.join();
#endif

    LOG(INFO) << "Main thread exiting...";
}

