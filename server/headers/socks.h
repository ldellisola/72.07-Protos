//
// Created by Lucas Dell'Isola on 30/05/2022.
//

#ifndef SERVER_SOCKS_H
#define SERVER_SOCKS_H
#include "tcp.h"

typedef enum {
    SOCKS5_CS_INIT,
    SOCKS5_CS_AUTH,
    SOCKS5_CA_READY
}SOCKS5_CONNECTION_STATUS;

typedef struct {
    SOCKS5_CONNECTION_STATUS Status;
    TcpSocket * Socket;


} Socks5Connection;

TcpSocket * InitSocks5Server(const char * port);

Socks5Connection * WaitForNewSocks5Connections(TcpSocket * server);

int HandleSocks5Request(Socks5Connection * socks5Connection,byte * buffer, size_t length);

int DisposeSocks5Connection(Socks5Connection* socks5Connection);

#endif //SERVER_SOCKS_H
