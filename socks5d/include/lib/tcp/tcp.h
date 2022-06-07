//
// Created by Lucas Dell'Isola on 28/05/2022.
//

#ifndef SERVER_TCP_H
#define SERVER_TCP_H


#include <stdbool.h>
#include <sys/socket.h>
#include "utils/utils.h"
#include <netdb.h>


typedef union {
    struct sockaddr_in6 ipv6;
    struct sockaddr_in ipv4;
    struct sockaddr base;
} TcpSocketAddress;

typedef struct {
    int FileDescriptor;
    int AddressFamily;
    TcpSocketAddress AddressInfo;
    bool IsPassive;
}TcpSocket;



TcpSocket  * InitTcpServer(const char * port);

TcpSocket * WaitForNewConnections(TcpSocket * tcpServer);

int DisposeTcpSocket(TcpSocket * socket);

size_t ReadFromTcpSocket(TcpSocket * socket, byte * buffer, int bufferLength);
size_t WriteToTcpSocket(TcpSocket * socket, byte * content, int contentLength);

int DisconnectFromTcpSocket(TcpSocket * socket);


#endif //SERVER_TCP_H
