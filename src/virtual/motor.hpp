#pragma once 

#include <functional>
#include <string>
#include <iomanip>
#include <easylogging++.h>

namespace robo {
namespace vir {
struct MotorBinder {
    std::function<void (float)> setTorque;
    float *angle;
    float *speed;
    void *actual;
};

class Motor {
public:
    explicit Motor(const std::string &name = "virtual"): _name(name) {}
    ~Motor() {
        if (!isBound()) {
            delete _angle;
            delete _speed;
        }
    }

    std::function<void (float)> setTorque = [this](float torque) { this->setTorqueDefault(torque); };

    bool isBound() const {
        return (_actual != nullptr);
    }
    void bind(const MotorBinder &binder) {
        if (!isBound()) {
            delete _angle;
            delete _speed;
        } else {
            LOG(WARNING) << "[Motor<" + _name + ">] Binding repeatedly!";
        }
        setTorque = binder.setTorque;
        _angle = binder.angle;
        _speed = binder.speed;
        _actual = binder.actual;
    }
    template <typename T>
    T *actual() const {
        return (T *)_actual;
    }
    float getAngle() const {
        return *_angle;
    }
    float getSpeed() const {
        return *_speed;
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
    const std::string _name;
    void *_actual {nullptr};
    float *_angle {new float {0.0f}};
    float *_speed {new float {0.0f}};

    void setTorqueDefault(float torque) {
        LOG(INFO) << std::setprecision(3) << "[Motor<" + _name + ">] set torque: " << torque << "Nm.";
    }
};
}
}

