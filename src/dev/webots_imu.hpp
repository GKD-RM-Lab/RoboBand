#pragma once
#ifdef USE_WEBOTS

#include <string>

#include "virtual/imu.hpp"
#include "dev/dev.hpp"

namespace robo {
namespace dev {
class WebotsImu: public Dev<io::Webots> {
public:
    explicit WebotsImu(io::Webots &webots_io, const std::string &inertial_name, const std::string &gyro_name);
    ~WebotsImu() = default;

    vir::ImuBinder binder {
        .angle = this->angle,
        .speed = this->speed
    };

private:
    webots::InertialUnit &inertial;
    webots::Gyro &gyro;

    float angle[3];
    float speed[3];

    virtual void update() override;

};
}
}

#endif

