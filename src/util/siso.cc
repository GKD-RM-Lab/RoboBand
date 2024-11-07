#include <numbers>

#include "util/siso.hpp"

namespace robo {
namespace util {
const SisoFunc rad_format = limit(std::numbers::pi_v<float>);

Pid::Pid(const PidParam& param): 
    Pid(param.kp,
        param.ki,
        param.kd,
        param.max_iout,
        param.max_out) {
}

Pid::Pid(const float kp,
         const float ki, 
         const float kd,
         const float max_iout,
         const float max_out):
    PidParam(kp, ki, kd, max_iout, max_out),
    SisoSys([&](float err) -> float {
        error[1] = error[0];
        error[0] = err;
        p_out = kp * error[0];
        i_out += ki * error[0];
        d_out = kd * (error[0] - error[1]);
        i_out << limit(max_iout);
        out = p_out + i_out + d_out;
        out << limit(max_out);
        return out;
    }) {
}

}
}
