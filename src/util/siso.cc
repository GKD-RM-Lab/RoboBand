#include "util/siso.hpp"

namespace robo {
namespace util {
Pid::Pid(const PidParam& param): 
    PidParam(param) {
}

Pid::Pid(const std::array<float, 5> &param):
    PidParam(param[0], param[1], param[2], param[3], param[4]) {
}

Pid::Pid(const float kp,
         const float ki, 
         const float kd,
         const float max_iout,
         const float max_out):
    PidParam(kp, ki, kd, max_iout, max_out) {
}

float Pid::calc(const float err) {
    error[1] = error[0];
    error[0] = err;
    p_out = kp * error[0];
    i_out += ki * error[0];
    d_out = kd * (error[0] - error[1]);
    i_out << limit(max_iout);
    out = p_out + i_out + d_out;
    out << limit(max_out);
    return out;
}
}
}
