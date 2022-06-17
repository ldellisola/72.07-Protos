//
// Created by Lucas Dell'Isola on 28/05/2022.
//

#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "utils/utils.h"


int printSocketAddress(int fd, char *addrBuffer) {

    struct sockaddr localAddr;
    socklen_t addrSize = sizeof(localAddr);

    if (getsockname(fd, (struct sockaddr *) &localAddr, &addrSize) < 0) {
        return -1;
    }

    void *numericAddress;

    in_port_t port;

    switch (localAddr.sa_family) {
        case AF_INET:
            numericAddress = &((struct sockaddr_in *) &localAddr)->sin_addr;
            port = ntohs(((struct sockaddr_in *) &localAddr)->sin_port);
            break;
        case AF_INET6:
            numericAddress = &((struct sockaddr_in6 *) &localAddr)->sin6_addr;
            port = ntohs(((struct sockaddr_in6 *) &localAddr)->sin6_port);
            break;
        default:
            strcpy(addrBuffer, "[unknown type]");    // Unhandled type
            return 0;
    }
    // Convert binary to printable address
    if (inet_ntop(localAddr.sa_family, numericAddress, addrBuffer, INET6_ADDRSTRLEN) == NULL)
        strcpy(addrBuffer, "[invalid address]");
    else {
        if (port != 0)
            sprintf(addrBuffer + strlen(addrBuffer), ":%u", port);
    }
    return 1;
}

const char *GetShutdownModeName(int shutdownMode) {
    switch (shutdownMode) {
        case SHUT_WR: return "write";
        case SHUT_RD: return "read";
        case SHUT_RDWR: return "read/write";
        default: return "unknown";
    }
}

int GetAddressFamily(const char *address) {
    struct in_addr in4Addr;
    if (1 == inet_pton(AF_INET,address,&in4Addr))
        return AF_INET;

    struct in6_addr in6Addr;
    if (1 == inet_pton(AF_INET6, address,&in6Addr))
        return AF_INET6;

    return AF_UNSPEC;
}

uint16_t GetPortNumberFromNetworkOrder(const byte port[2]) {
    uint16_t n =  port[0];
    return (n << 8) | port[1];
}



