//
// Created by Lucas Dell'Isola on 28/05/2022.
//

#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
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


const char *GetPortFromAddress(struct sockaddr_storage *address, char * buffer, size_t bufferSize){
    in_port_t port;
    switch (address->ss_family) {
        case AF_INET:
            port = ntohs(((struct sockaddr_in *) &address)->sin_port);
            break;
        case AF_INET6:
            port = ntohs(((struct sockaddr_in6 *) &address)->sin6_port);
            break;
        default:
            return "Unknown";
    }

    snprintf(buffer, bufferSize,"%u", port);
    return buffer;
}

const char *GetIPFromAddress(struct sockaddr_storage *address, char * buffer, size_t bufferSize) {
    void *numericAddress;

    switch (address->ss_family) {
        case AF_INET:
            numericAddress = &((struct sockaddr_in *)address)->sin_addr;
            break;
        case AF_INET6:
            numericAddress = &((struct sockaddr_in6 *)address)->sin6_addr;
            break;
        default:
            return "Unknown";
    }

    if (inet_ntop(address->ss_family, numericAddress, buffer, bufferSize) == NULL)
        strcpy(buffer, "[invalid address]");

    return buffer;
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

void PrintAccessLog(const char * username, struct sockaddr_storage * src,byte * destAddress,int destAddressType,byte destPort[2],int command) {
    char timestamp[] = "YYYY-MM-ddTHH:mm:ssz";
    struct timeval tv;
    struct tm tm;
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tm);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%Sz", &tm);

    if (null == username)
        username = "unknown";

    char srcAddress[INET6_ADDRSTRLEN + 1],srcPort[10];
    GetIPFromAddress(src, srcAddress, sizeof(srcAddress));
    GetPortFromAddress(src, srcPort, sizeof(srcPort));

    char destAddressBuffer[INET6_ADDRSTRLEN +1];
    if (AF_UNSPEC != destAddressType)
        destAddress = (byte *) inet_ntop(destAddressType,destAddress,destAddressBuffer,INET6_ADDRSTRLEN);

    printf("%s\t%s\tA\t%s\t%s\t%s\t%d\t%d\n",
           timestamp,
           username,
           srcAddress,
           srcPort,
           destAddress,
           GetPortNumberFromNetworkOrder(destPort),
           command
    );

}



