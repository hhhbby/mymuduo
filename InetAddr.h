#pragma once

#include <netinet/in.h>
#include <string>

class InetAddr {
public:
    InetAddr(sockaddr_in addr);
    InetAddr(uint16_t port, std::string ip = "127.0.0.1");

    uint16_t toPort();
    std::string toIpPort();
    sockaddr_in* addr();
private:
    sockaddr_in addr_;
};