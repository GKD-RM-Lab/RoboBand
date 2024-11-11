#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include "io/socket.hpp"

namespace robo {
namespace io {
Socket::Socket(const std::string &name, const int port, const int buffer_size, const float timeout_ms):
    IoKey(name, buffer_size),
    sockfd(::socket(AF_INET, SOCK_DGRAM, 0)) {
    if (sockfd < 0) {
        LOG(ERROR) << "[Socket<" + name + ">] Error " << errno << " from open: " << std::strerror(errno);
        exit(errno);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = ::htons(port);

    if (::bind(sockfd, (sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
        LOG(ERROR) << "[Socket<" + name + ">] Cannot bind socket fd with port number" << port << ".";
        LOG(ERROR) << "[Socket<" + name + ">] Error " << errno << " from bind: " << std::strerror(errno);
        exit(errno);
    }

    if (timeout_ms > 0) {
        struct timeval tv;
        tv.tv_sec = (int)(timeout_ms / 1000);
        tv.tv_usec = (int)(timeout_ms * 1000) % 1000000;
        if (::setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
            LOG(ERROR) << "[Socket<" + name + ">] Error " << errno << " from setsockopt: " << std::strerror(errno);
            exit(errno);
        }
    }
}

Socket::~Socket() {
    if (::close(sockfd) < 0) {
        LOG(ERROR) << "[Socket<" + name + ">] Error " << errno << " from close: " << std::strerror(errno);
    }
}

int Socket::read(std::tuple<in_addr_t, int> &key, uint8_t *data) {
    sockaddr_in cli_addr;
    socklen_t cli_addr_len = sizeof(cli_addr);
    auto len = ::recvfrom(sockfd, data, 256, MSG_WAITALL, (sockaddr *)&cli_addr, &cli_addr_len);
    if (len > 0) {
        int port = ::ntohs(cli_addr.sin_port);
        key = std::make_tuple(cli_addr.sin_addr.s_addr, port);
    } else if (len == 0) {
        LOG(WARNING) << "[Socket<" + name + ">] Read timeout.";
    } else {
        LOG(ERROR) << "[Socket<" << name << ">] Error " << errno << " from recvfrom: " << std::strerror(errno);
    }
    return len;
}

bool Socket::send(const std::tuple<in_addr_t, int> &key, const uint8_t *message, const int len) {
    in_addr_t ip = std::get<0>(key);
    int port = std::get<1>(key);

    sockaddr_in to_addr;
    memset(&to_addr, 0, sizeof(to_addr));
    to_addr.sin_family = AF_INET;
    to_addr.sin_port = ::htons(port);
    to_addr.sin_addr.s_addr = ip;

    auto n = ::sendto(sockfd, message, len, MSG_CONFIRM, reinterpret_cast<const sockaddr*>(&to_addr), sizeof(to_addr));
    if (n < 0) {
        LOG(ERROR) << "[Socket<" << name << ">] Error " << errno << " from sendto: " << std::strerror(errno);
        LOG(WARNING) << "[Socket<" + name + ">] Failed to send to client, key: " << ::inet_ntoa(to_addr.sin_addr) << ":" << port << ".";
        return false;
    }

    return true;
}
}
}

