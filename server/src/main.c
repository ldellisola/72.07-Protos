#include <stdio.h>
#include <netdb.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
#include "../headers/cli.h"

#define null NULL
int servSock;

void SIGIOHandler(int signalType); // Handle SIGIO

int main(int argc, char ** argv) {
    setvbuf(stdout, NULL, _IONBF, 0);

    CliArguments arguments = ParseCli(argc,argv);


    struct addrinfo addrCriteria;                   // Criteria for address
    memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
    addrCriteria.ai_family = AF_INET;             // Any address family
    addrCriteria.ai_flags = AI_PASSIVE;             // Accept on any address/port
    addrCriteria.ai_socktype = SOCK_STREAM;          // Only datagram sockets
    addrCriteria.ai_protocol = IPPROTO_TCP;

    struct addrinfo * servAddr;
    int retval = getaddrinfo(null,arguments.Port,&addrCriteria,&servAddr);

    if (retval != 0){
        perror("getaddrinfo");
        return 1;
    }

    servSock = socket(servAddr->ai_family,servAddr->ai_socktype, servAddr-> ai_protocol);

    if (servSock < 0) {
        perror("socket");
        return 1;
    }

    if (bind(servSock, servAddr->ai_addr, servAddr-> ai_addrlen) < 0){
        perror("bind");
        close(servSock);
    }

    if (listen(servSock, 10) < 0)
        perror("adsdasda");

    freeaddrinfo(servAddr);

    struct sockaddr sockAddrIn;
    socklen_t sockAddrInSize = sizeof(sockAddrIn);

    int newSocket = accept(servSock,&sockAddrIn,&sockAddrInSize);

    if (newSocket < 0) {
        perror("connect");
        close(servSock);
        return 1;
    }

    do{
        char buffer[500];
        size_t bytes =  recvfrom(newSocket,buffer,500,0,(struct sockaddr *)&sockAddrIn,&sockAddrInSize);
        if (bytes < 0)
        {
            perror("recvfrom");
            close(servSock);
            close(newSocket);
            return 1;
        }

        buffer[bytes] = 0;
        printf(buffer);
    } while (true);


    return 0;
}

