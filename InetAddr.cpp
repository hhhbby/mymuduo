#include "InetAddr.h"
#include "Logger.h"

#include <arpa/inet.h>
#include <strings.h>
#include <string.h>

InetAddr::InetAddr(uint16_t port, std::string ip)
{
    bzero(&addr_, sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr) != 1)
        LOG_ERROR("InetAddr-inet_pton failed");
}

uint16_t InetAddr::toPort()
{
    return ntohs(addr_.sin_port);
}

std::string InetAddr::toIpPort()
{
    char buf[64]{};
    if (!inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof buf))
        LOG_ERROR("InetAddr-inet_ntop failed");
    int sz = strlen(buf);
    snprintf(buf + sz, (sizeof buf) - sz, ":%u",toPort());
    return buf;
}

// #include <iostream>
// int main()
// {
//     std::cout << InetAddr(2222, "192.168.0.22").toIpPort() << std::endl;
// }