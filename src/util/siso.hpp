#pragma once

#include <numbers>
#include <functional>
#include <type_traits>

namespace robo {
namespace util {
using SisoFunc = std::function<float (float)>;

class SisoSys {
public:
    explicit SisoSys() {}
    virtual ~SisoSys() = default;

    virtual float calc(float input) = 0;

    float operator()(float input) {
        return calc(input);
    }
};

struct Out {
    Out(float o): o(o) {}
    float o;

    operator float() {
        return o;
    }
    Out &operator>>(float &next) {
        o += next;
        return *this;
    }
};

template <typename T>
constexpr decltype(auto) is_siso() {
    return std::is_same_v<T, SisoFunc> || std::is_base_of_v<SisoSys, T>;
};
template <typename T>
using siso_t = typename std::enable_if_t<is_siso<T>()>;

template <typename T, typename = siso_t<T>>
inline Out operator>>(float input, T siso) {
    return siso(input);
}

template <typename T, typename = siso_t<T>>
inline float &operator<<(float &input, T siso) {
    input = siso(input);
    return input;
}

template <typename T, typename = siso_t<T>>
inline SisoFunc operator+(float sys_out, T siso) {
    return [=](float input) -> float {
        return sys_out + siso(input);
    };
};

template <typename T, typename = siso_t<T>>
inline SisoFunc operator+(T siso, float sys_out) {
    return sys_out + siso; 
};

template <typename T, typename U, typename = typename std::enable_if_t<is_siso<T>() && is_siso<U>()>>
inline SisoFunc operator+(T siso1, U siso2) {
    return [=](float input) -> float {
        return siso1(input) + siso2(input);
    };
};

template <typename T, typename U, typename = typename std::enable_if_t<is_siso<T>() && is_siso<U>()>>
inline SisoFunc operator>>(T siso1, U siso2) {
    return [=](float input) -> float {
        return input >> siso1 >> siso2;
    };
}

inline constexpr SisoFunc limit(float min, float max) {
    return [=](float input) -> float {
        if (input < min)
            input = min;
        if (input > max)
            input = max;
        return input;
    };
};
inline constexpr SisoFunc limit(float n) {
    return limit(-n, n);
};
inline const SisoFunc rad_format {
    [](float input) -> float {
        static constexpr auto pi = std::numbers::pi_v<float>;
        if (input < -pi)
            input += 2 * pi;
        if (input >= pi)
            input -= 2 * pi;
        return input;
    }
};

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
    explicit Pid(const std::array<float, 5> &param);
    explicit Pid(const float kp,
                 const float ki, 
                 const float kd,
                 const float max_iout,
                 const float max_out);

    virtual float calc(float err) override;

private:
    float p_out {0.0f};
    float i_out {0.0f};
    float d_out {0.0f};
    float out {0.0f};
    float error[2] {0.0f};
};
}
}

