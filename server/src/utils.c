//
// Created by Lucas Dell'Isola on 28/05/2022.
//

#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdarg.h>
#include "../headers/utils.h"
#include "../headers/tcp.h"

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


