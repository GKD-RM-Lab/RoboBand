#include <ctime>
#include <pthread.h>

#include "util/util.hpp"

namespace robo {
namespace util {
/**sys**/
struct sched_attr {
    __u32 size;

    __u32 sched_policy;
    __u64 sched_flags;

    /* SCHED_NORMAL, SCHED_BATCH */
    __s32 sched_nice;

    /* SCHED_FIFO, SCHED_RR */
    __u32 sched_priority;

    /* SCHED_DEADLINE (nsec) */
    __u64 sched_runtime;
    __u64 sched_deadline;
    __u64 sched_period;
};

int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags)
{
    return syscall(__NR_sched_setattr, pid, attr, flags);
}

int sched_getattr(pid_t pid, struct sched_attr *attr, unsigned int size, unsigned int flags)
{
    return syscall(__NR_sched_getattr, pid, attr, size, flags);
}

int setThisThreadRealTime(long period_ns) {
    struct sched_attr attr;

    attr.size = sizeof(attr);
    attr.sched_flags = 0;
    attr.sched_nice = 0;
    attr.sched_priority = 0;

    attr.sched_policy = SCHED_DEADLINE;
    attr.sched_runtime = period_ns / 1000 * 300;
    attr.sched_deadline = period_ns / 1000 * 500;
    attr.sched_period = period_ns;

    return sched_setattr(syscall(SYS_gettid), &attr, 0);
}

/**toml**/
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

/**io**/
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
