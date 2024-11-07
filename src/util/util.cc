#include <stdexcept>
#include "util/util.hpp"

namespace robo {
namespace util {
void mergeConfig(toml::table& user_config, toml::table& default_config) {
    for (const auto& [key, value] : user_config) {
        if (auto* user_table = value.as_table()) {
            if (auto* default_table = default_config[key].as_table()) {
                mergeConfig(*default_table, *user_table);
            } else {
                default_config.insert_or_assign(key, value);
            }
        } else {
            default_config.insert_or_assign(key, value);
        }
    }
}

const toml::table &getTable(const toml::table &table, const std::string &key) {
    auto new_table = table[key].as_table();
    if (!!new_table) {
        LOG(INFO) << "True key: " + key;
        return *new_table;
    } else {
        LOG(ERROR) << R"(An error occurred when took out a subtable named ")" + key + R"(".)";
        throw std::runtime_error("Wrong configuration file.");
        return table;
    }
}
const toml::table &operator>(const toml::table &table, const std::string &key) {
    return getTable(table, key);
}

in_addr_t to_in_addr(const std::string &host) {
    in_addr_t ip;
    addrinfo hints, *res;
    int status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if ((status = getaddrinfo(host.c_str(), nullptr, &hints, &res)) != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return INADDR_NONE;
    }

    sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in *>(res->ai_addr);
    ip = ipv4->sin_addr.s_addr;

    if (res->ai_next != nullptr) {
        std::clog << R"(The host ")" + host + R"(" corresponds to multiple IP addresses)";
    }

    freeaddrinfo(res);

    return ip;
}

}
}
