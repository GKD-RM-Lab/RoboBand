#pragma once

#include <iostream>
#include <string>
#include <array>
#include <type_traits>
#include <typeinfo>
#include <cxxabi.h>
#include <toml++/impl/forward_declarations.hpp>
#include <toml++/toml.hpp>
#include <easylogging++.h>
#include <netdb.h>

namespace robo {
namespace util {
/**math**/ 
template <typename T>
inline T abs_limit(T x, const T lim) {
    if (x < -lim) {
        x = -lim;
    }
    if (x > lim) {
        x = lim;
    }
    return x;
}

/**toml**/
void mergeConfig(toml::table& user_config, toml::table& default_config);

const toml::table &getTable(const toml::table &table, const std::string &key);
const toml::table &operator>(const toml::table &table, const std::string &key);

template <typename T>
T getValue(const toml::table &table, const std::string &key) {
    auto value = table[key].value<T>();
    if (!!value) {
        LOG(INFO) << "True key: " + key;
        return value.value();
    } else {
        LOG(ERROR) << R"(An error occurred when got a value named ")" + key + R"(".)";
        throw std::runtime_error("Wrong configuration file.");
        return *new T;
    }
}

template <typename T, std::size_t N>
std::array<T, N> getArray(const toml::table &table, const std::string &key) {
    std::array<T, N> arr;
    auto tarr = table[key].as_array();

    if (!tarr) {
        LOG(ERROR) << R"(An error occurred when got an array named ")" + key + R"(".)";
        throw std::runtime_error("Wrong configuration file.");
        return arr;
    }

    if (auto tarr_size = tarr->size(); tarr_size!= N) {
        LOG(ERROR) << R"(Array named ")" + key + R"(" required )" << N << " elements, but you gave " << tarr_size << ".";
        throw std::runtime_error("Wrong configuration file.");
        return arr;
    }

    int i = 0;
    for (auto &&elem: *tarr) {
        int status;
        bool is_vaild = elem.visit([&arr, i](auto &&el) noexcept -> bool {
            if constexpr (toml::is_number<decltype(el)> && (std::is_same<T, int>() || std::is_same<T, float>())) {
                arr[i] = static_cast<T>(*el);
            } else if constexpr (toml::is_string<decltype(el)> && (std::is_same<T, std::string>())) {
                arr[i] = static_cast<T>(*el);
            } else if constexpr (toml::is_boolean<decltype(el)> && (std::is_same<T, bool>())) {
                arr[i] = static_cast<T>(*el);
            } else {
                return false;
            }
            return true;
        });

        if (!is_vaild) {
            LOG(ERROR) << R"(Array named ")" + key + R"(" required ")"
                       << abi::__cxa_demangle(typeid(T).name(), 0, 0, &status)
                       << R"(" type data, but you gave ")"
                       << abi::__cxa_demangle(typeid(elem).name(), 0, 0, &status)
                       << R"(".)";
            throw std::runtime_error("Wrong configuration file.");
            return arr;
        }
    
        i++;
    }

    LOG(INFO) << "True key: " + key;
    return arr;
}

template <typename T>
constexpr decltype(auto) is_param() {
    return std::is_same_v<T, int> ||
           std::is_same_v<T, float> ||
           std::is_same_v<T, std::string> ||
           std::is_same_v<T, toml::table>;
};
template <typename T>
using param_t = typename std::enable_if_t<is_param<T>()>;

template <typename T = typename toml::table, std::size_t N = 0, typename = param_t<T>>
struct Key {
    Key(const std::string &key): key(key) {}
    using type_T = T;
    static constexpr std::size_t type_N = N;
    using type = std::array<T, N>;
    const std::string key;
};
template <typename T>
struct Key<T, 0> {
    Key(const std::string &key): key(key) {}
    using type_T = T;
    static constexpr std::size_t type_N = 0;
    using type = T;
    const std::string key;
};

template <typename K, typename = std::enable_if_t<std::is_base_of_v<K, Key<typename K::type_T, K::type_N>>>>
decltype(auto) operator>(const toml::table &table, const K key) {
    if constexpr (std::is_same_v<typename K::type, int> ||
                  std::is_same_v<typename K::type, float> ||
                  std::is_same_v<typename K::type, std::string>) {
        return getValue<typename K::type>(table, key.key);
    } else if constexpr (std::is_same_v<typename K::type_T, toml::table>) {
        return getTable<typename K::type>(table, key.key);
    } else {
        return getArray<typename K::type_T, K::type_N>(table, key.key);
    }
}

/**io**/
in_addr_t to_in_addr(const std::string &host);

/**iostream**/
template<typename T, typename = void>
struct is_streamable : std::false_type {};

template<typename T>
struct is_streamable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>> : std::true_type {};
}
}

using robo::util::operator>;

