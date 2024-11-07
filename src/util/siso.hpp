#pragma once

#include <functional>
#include <type_traits>

namespace robo {
namespace util {
using SisoFunc = std::function<float (float)>;

class SisoSys {
public:
    explicit SisoSys(const SisoFunc &calc): calc(calc) {}
    virtual ~SisoSys() = default;

    const SisoFunc calc;

    float operator()(float input) {
        return calc(input);
    }
};

template <typename T>
constexpr decltype(auto) is_siso() {
    return std::is_same_v<T, SisoFunc> || std::is_base_of_v<SisoSys, T>;
};
template <typename T>
using siso_t = typename std::enable_if_t<is_siso<T>()>;

template <typename T, typename = siso_t<T>>
inline float operator>>(float input, const T &siso) {
    return siso(input);
}

template <typename T, typename = siso_t<T>>
inline float &operator<<(float &input, const T &siso) {
    input = siso(input);
    return input;
}

template <typename T, typename = siso_t<T>>
inline const SisoFunc operator+(float sys_out, const T &siso) {
    return [&](float input) -> float {
        return sys_out + siso(input);
    };
};

template <typename T, typename = siso_t<T>>
inline const SisoFunc operator+(const T &siso, float sys_out) {
    return sys_out + siso; 
};

template <typename T, typename U, typename = typename std::enable_if_t<is_siso<T>() && is_siso<U>()>>
inline const SisoFunc operator+(const T &siso1, const U &siso2) {
    return [&](float input) -> float {
        return siso1(input) + siso2(input);
    };
};

template <typename T, typename U, typename = typename std::enable_if_t<is_siso<T>() && is_siso<U>()>>
inline const SisoFunc operator>>(const T &siso1, const U &siso2) {
    return [&](float input) -> float {
        return input >> siso1 >> siso2;
    };
}

inline const SisoFunc limit(float min, float max) {
    return [=](float input) -> float {
        if (input < min)
            input = min;
        if (input > max)
            input = max;
        return input;
    };
};
inline const SisoFunc limit(float n) {
    return limit(-n, n);
};
extern const SisoFunc rad_format;

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

