#pragma once

#include <functional>

namespace robo {
namespace util {
using SisoFunc = std::function<float (float)>;

inline float operator>>(float input, const SisoFunc &calc) {
    return calc(input);
}

inline float &operator<<(float &input, const SisoFunc &calc) {
    input = calc(input);
    return input;
}

inline SisoFunc operator+(float sys_out, const SisoFunc &calc) {
    return [&](float input) -> float {
        return sys_out + calc(input);
    };
};
inline SisoFunc operator+(const SisoFunc &calc, float sys_out) {
    return sys_out + calc; 
};
inline SisoFunc operator+(const SisoFunc &calc1, const SisoFunc &calc2) {
    return [&](float input) -> float {
        return calc1(input) + calc2(input);
    };
};

class SisoSys {
public:
    explicit SisoSys(const SisoFunc &calc): calc(calc) {}
    virtual ~SisoSys() = default;

    const SisoFunc calc;

    float operator()(float input) {
        return calc(input);
    }

    friend float operator>>(float input, SisoSys &sys) {
        return sys.calc(input);
    }

    friend float &operator<<(float &input, SisoSys &sys) {
        input = sys.calc(input);
        return input;
    }

    SisoFunc operator+(float sys_out) {
        return [&](float input) -> float {
            return calc(input) + sys_out;
        };
    }
    friend SisoFunc operator+(float sys_out, SisoSys &sys) {
        return sys + sys_out;
    }
    SisoFunc operator+(const SisoFunc &func) {
        return [&](float input) -> float {
            return func(input) + calc(input);
        };
    }
    friend SisoFunc operator+(const SisoFunc &func, SisoSys &sys) {
        return sys + func;
    }
    friend SisoFunc operator+(SisoSys &sys1, SisoSys &sys2) {
        return [&](float input) -> float {
            return sys1.calc(input) + sys2.calc(input);
        };
    }
};

template <typename T, typename U>
inline SisoFunc operator>>(T &siso1, U &siso2) {
    return [&](float input) -> float {
        return input >> siso1 >> siso2;
    };
}

inline SisoFunc limit(float min, float max) {
    return [=](float input) -> float {
        if (input < min)
            input = min;
        if (input > max)
            input = max;
        return input;
    };
};
inline SisoFunc limit(float n) {
    return limit(-n, n);
};
extern SisoFunc rad_format;

struct PidParam {
    float kp;
    float ki;
    float kd;
    float max_iout;
    float max_out;
};

class Pid: public PidParam, public SisoSys {
public:
    explicit Pid(const PidParam &param);
    explicit Pid(const float kp,
                 const float ki, 
                 const float kd,
                 const float max_iout,
                 const float max_out);

private:
    float p_out {0.0f};
    float i_out {0.0f};
    float d_out {0.0f};
    float out {0.0f};
    float error[2] {0.0f};
};
}
}

using robo::util::operator>>;
using robo::util::operator+;

