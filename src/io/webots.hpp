#pragma once
#ifdef USE_WEBOTS

#include <atomic>
#include <barrier>
#include <vector>
#include <functional>
#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/PositionSensor.hpp>
#include <webots/InertialUnit.hpp>
#include <webots/Gyro.hpp>

namespace robo {
namespace io {
class Webots {
public:
    explicit Webots();
    ~Webots();

    webots::Robot robot;
    std::atomic<int> time;
    std::atomic<int> bind_runner_num {0};
    std::barrier<> *sync_point {nullptr};
    std::vector<std::function<void ()>> updaters;

    int step();
    void motorSetTorque(webots::Motor &motor, const float torque);
    void encoderGetValue(webots::PositionSensor &encoder, float &value, int &time);
    void inertialGetRollPitchYaw(webots::InertialUnit &inertial, float *angle);
    void gyroGetValues(webots::Gyro &gyro, float *speed);

private:
    int basic_time_step;

};
}
}

#endif
