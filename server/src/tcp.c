//
// Created by Lucas Dell'Isola on 28/05/2022.
//

#include <memory.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include "../headers/tcp.h"
#include "../headers/utils.h"



TcpServer * InitTcpServer(char * port ){

    if (port == null)
        return null;

    TcpServer * tcpServer = malloc(sizeof(TcpServer));
    bzero(tcpServer,sizeof(TcpServer));
    tcpServer->Port = port;

    struct addrinfo addrCriteria;                   // Criteria for address
    memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
    addrCriteria.ai_family = AF_UNSPEC;             // Any address family
    addrCriteria.ai_flags = AI_PASSIVE;             // Accept on any address/port
    addrCriteria.ai_socktype = SOCK_STREAM;          // Only datagram sockets
    addrCriteria.ai_protocol = IPPROTO_TCP;

    struct addrinfo * servAddr;
    if ( getaddrinfo(null,tcpServer->Port,&addrCriteria,&servAddr) != 0){
        perror("Getting all the interfaces...");
        return null;
    }

    // TODO: allow multiple
    int servSock = socket(servAddr->ai_family,servAddr->ai_socktype, servAddr-> ai_protocol);

    if (servSock < 0) {
        perror("Creating a passive socket");
        return null;
    }

    if (bind(servSock, servAddr->ai_addr, servAddr-> ai_addrlen) < 0){
        perror("Binding the passive socket");
        DisposeTcpServer(tcpServer);
        return null;
    }

    char aa[1000];
    printSocketAddress(servSock,aa);
    printf("%s",aa);

    if (listen(servSock, 10) < 0) {
        perror("Setting the passive socket to listen");
        DisposeTcpServer(tcpServer);
        return null;
    }

    freeaddrinfo(servAddr);

    tcpServer->PassiveSocket = servSock;

    return tcpServer;
}


int WaitForNewConnections(TcpServer * tcpServer, TcpSocket * tcpSocket){
    if(tcpServer == null)
        return ERROR;

    struct sockaddr sockAddrIn;
    socklen_t sockAddrInSize = sizeof(sockAddrIn);

    int newSocket = accept(tcpServer->PassiveSocket,(struct sockaddr*)&sockAddrIn,&sockAddrInSize);

    if (newSocket < 0) {
        perror("Opening a new socket");
        close(tcpServer->PassiveSocket);
        return ERROR;
    }

    char aa[1000];
    printSocketAddress(newSocket,aa);
    printf("%s",aa);

    tcpSocket->FileDescriptor = newSocket;
    tcpSocket->AddressFamily = sockAddrIn.sa_family;
    tcpSocket->AddressInfo.base = sockAddrIn;

    return newSocket;
}

int DisposeTcpServer(TcpServer * tcpServer){
    if (tcpServer == null)
        return ERROR;

    if (close(tcpServer->PassiveSocket) < 0)
        return ERROR;

    free(tcpServer);
    return OK;
}

int DisposeTcpSocket(TcpSocket * socket){
    if (socket == null)
        return ERROR;

    if (close(socket->FileDescriptor) < 0)
        return ERROR;
    return OK;
}

int ReadFromTcpSocket(TcpSocket * socket, char * buffer, int bufferLength){
    size_t bytes =  recv(socket->FileDescriptor,buffer,bufferLength,0);
    return bytes;
}

int WriteToTcpSocket(TcpSocket * socket, char * content, int contentLength){
    size_t bytes = send(socket->FileDescriptor,content,contentLength,0);
    return bytes;
}
