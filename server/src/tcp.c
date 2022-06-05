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
#include "../headers/logger.h"


TcpSocket * InitTcpServer(const char * port ){
    LogInfo("Staring TCP server...");
    if (port == null) {
        LogError(false,"Invalid port. Cannot be null");
        return null;
    }

    struct addrinfo addrCriteria;                   // Criteria for address
    memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
    addrCriteria.ai_family = AF_UNSPEC;             // Any address family
    addrCriteria.ai_flags = AI_PASSIVE;             // Accept on any address/port
    addrCriteria.ai_socktype = SOCK_STREAM;          // Only datagram sockets
    addrCriteria.ai_protocol = IPPROTO_TCP;

    struct addrinfo * servAddr;
    if ( getaddrinfo(null,port,&addrCriteria,&servAddr) != 0){
        LogError(true,"Cannot get local addresses");
        return null;
    }

    // TODO: allow multiple
    int servSock = socket(servAddr->ai_family,servAddr->ai_socktype, servAddr-> ai_protocol);

    if (servSock < 0) {
        LogError(true,"Cannot open passive socket");
        return null;
    }

    LogInfo("Opened passive socket for TCP server");

    if (bind(servSock, servAddr->ai_addr, servAddr-> ai_addrlen) < 0){
        LogError(true,"Cannot bind socket to file descriptor");
        close(servSock);
        return null;
    }


    LogInfo("Bound TCP server socket to file descriptor");

    if (listen(servSock, 10) < 0) {
        LogError(true,"Cannot set passive socket to listen");
        close(servSock);
        return null;
    }

    LogInfo("Set TCP server socket to listen");

    freeaddrinfo(servAddr);

    struct sockaddr localAddr;
    socklen_t addrSize = sizeof(localAddr);

    if (getsockname(servSock, (struct sockaddr *) &localAddr, &addrSize) < 0) {
        LogError(true,"Cannot get server socket name");
        close(servSock);
        return null;
    }

    TcpSocket * tcpServer = calloc(1, sizeof(TcpSocket));
    tcpServer->AddressFamily = servAddr->ai_family;
    tcpServer->AddressInfo.base = localAddr;
    tcpServer->FileDescriptor = servSock;
    tcpServer->IsPassive = true;

    LogInfo("TCP server up and running...");

    return tcpServer;
}


TcpSocket *  WaitForNewConnections(TcpSocket * tcpServer){
    LogInfo("Waiting for new TCP connections...");
    if(tcpServer == null) {
        LogError(false,"Invalid TCP server. Cannot be null");
        return null;
    }

    struct sockaddr sockAddrIn;
    socklen_t sockAddrInSize = sizeof(sockAddrIn);

    int newSocket = accept(tcpServer->FileDescriptor,(struct sockaddr*)&sockAddrIn,&sockAddrInSize);
    if (newSocket < 0) {
        LogError(true,"Cannot open active socket for new connection");
        return null;
    }

    TcpSocket * tcpSocket = calloc(1, sizeof(TcpSocket));
    tcpSocket->FileDescriptor = newSocket;
    tcpSocket->AddressFamily = sockAddrIn.sa_family;
    tcpSocket->AddressInfo.base = sockAddrIn;
    tcpSocket->IsPassive = false;

    LogInfo("New TCP connection up and running on file descriptor %d",newSocket);

    return tcpSocket;
}

int DisposeTcpSocket(TcpSocket * socket){
    if (socket == null) {
        LogError(false,"Failed to dispose TCP socket. Cannot be null");
        return ERROR;
    }

    LogInfo("Disposing TCP socket on file descriptor %d",socket->FileDescriptor);

    if (close(socket->FileDescriptor) < 0){
        LogError(true,"Cannot close file descriptor %d",socket->FileDescriptor);
        return ERROR;
    }

    LogInfo("File descriptor %d closed successfully");

    free(socket);

    LogInfo("TCP socket disposed successfully");

    return OK;
}

int DisconnectFromTcpSocket(TcpSocket * socket){
    if (socket == null)
    {
        LogError(false,"Failed to disconnect TCP socket. Cannot be null");
        return ERROR;
    }

    LogInfo("Disconnecting from TCP socket on file descriptor %d...",socket->FileDescriptor);

    if (shutdown(socket->FileDescriptor,SHUT_RDWR) < 0){
        LogError(true,"Cannot close TCP socket on file descriptor %d...",socket->FileDescriptor);
        return ERROR;
    }

    LogInfo("Successfully disconnected from TCP socket on file descriptor %d!",socket->FileDescriptor);

    return OK;
}

size_t ReadFromTcpSocket(TcpSocket * socket, byte * buffer, int bufferLength){
    if(socket == null) {
        LogError(false,"Cannot read from null TCP socket");
        return ERROR;
    }

    if (buffer == null)
    {
        LogError(false,"TCP read buffer cannot be null");
        return ERROR;
    }

    LogInfo("Reading from TCP socket on file descriptor %d...",socket->FileDescriptor);
    size_t bytes =  recv(socket->FileDescriptor,buffer,bufferLength,0);

    if (bytes < 0){
        LogError(true,"Could not read from TCP socket on file descriptor %d",socket->FileDescriptor);
        return ERROR;
    }

    LogInfo("Read %d bytes from TCP socket on file descriptor %d",bytes,socket->FileDescriptor);
    return bytes;
}

size_t WriteToTcpSocket(TcpSocket * socket, byte * content, int contentLength){
    if(socket == null) {
        LogError(false,"Cannot write to null TCP socket");
        return ERROR;
    }

    if (content == null)
    {
        LogError(false,"TCP write buffer cannot be null");
        return ERROR;
    }

    LogInfo("Writing %d bytes to TCP socket on file descriptor %d...",contentLength, socket->FileDescriptor);

    size_t bytes = send(socket->FileDescriptor,content,contentLength,0);

    if(bytes < 0){
        LogError(true,"Could not write to TCP socket on file descriptor %d",socket->FileDescriptor);
        return ERROR;
    }

    LogInfo("Wrote %d bytes to TCP socket on file descriptor %d",bytes,socket->FileDescriptor);

    return bytes;
}
