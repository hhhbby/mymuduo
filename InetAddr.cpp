#include "InetAddr.h"
#include <arpa/inet.h>
#include <strings.h>

InetAddr::InetAddr(uint16_t port, std::string ip)
{
    bzero(&addr_, sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
}

uint16_t InetAddr::toPort()
{
    return ntohs(addr_.sin_port);
}

std::string InetAddr::toIpPort()
{
    char buf[64]{};
    snprintf(buf, sizeof buf, "%s:%u", inet_ntoa(addr_.sin_addr), toPort());
    return buf;
}

// #include <iostream>
// int main()
// {
//     std::cout << InetAddr(2222, "192.168.0.22").toIpPort() << std::endl;
// }