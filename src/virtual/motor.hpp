#pragma once 

#include <functional>
#include <string>
#include <iomanip>
#include <easylogging++.h>

#include "util/siso.hpp"

namespace robo {
namespace vir {
struct MotorBinder {
    std::function<void (float)> setTorque;
    float *angle;
    float *speed;
};

class Motor {
public:
    explicit Motor(const std::string &name = "virtual"): name(name) {}
    ~Motor() {
        if (!isBound()) {
            delete angle;
            delete speed;
        }
    }

    std::function<void (float)> setTorque = [this](float torque) { this->setTorqueDefault(torque); };

    bool isBound() const {
        return is_bound;
    }
    void bind(const MotorBinder &binder) {
        if (!isBound()) {
            delete angle;
            delete speed;
        } else {
            LOG(WARNING) << "[Motor<" + name + ">] Binding repeatedly!";
        }
        setTorque = binder.setTorque;
        angle = binder.angle;
        speed = binder.speed;
        is_bound = true;
    }
    float getAngle() const {
        return *angle;
    }
    float getSpeed() const {
        return *speed;
    }
    friend float operator>>(float torque, Motor &motor) {
        motor.setTorque(torque);
        return torque;
    }
    friend float *operator>>(float *torque, Motor &motor) {
        motor.setTorque(*torque);
        return torque++;
    }

private:
    const std::string name;
    bool is_bound {false};
    float *angle {new float {0.0f}};
    float *speed {new float {0.0f}};

    void setTorqueDefault(float torque) {
        LOG(INFO) << std::setprecision(3) << "[Motor<" + name + ">] set torque: " << torque << "Nm.";
    }
};
}
}

