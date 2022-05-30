//
// Created by Lucas Dell'Isola on 28/05/2022.
//

#ifndef SERVER_TCP_H
#define SERVER_TCP_H

typedef struct {
    int PassiveSocket;
    const char * Port;
} TcpServer;


typedef union {
    struct sockaddr_in6 ipv6;
    struct sockaddr_in ipv4;
    struct sockaddr base;
} TcpSocketAddress;

typedef struct {
    int FileDescriptor;
    int AddressFamily;
    TcpSocketAddress AddressInfo;
}TcpSocket;



TcpServer* InitTcpServer(char * port);

int WaitForNewConnections(TcpServer * tcpServer, TcpSocket * socket);

int DisposeTcpServer(TcpServer * tcpServer);

int DisposeTcpSocket(TcpSocket * socket);

int ReadFromTcpSocket(TcpSocket * socket, char * buffer, int bufferLength);
int WriteToTcpSocket(TcpSocket * socket, char * content, int contentLength);


#endif //SERVER_TCP_H
