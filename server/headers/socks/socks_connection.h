//
// Created by Lucas Dell'Isola on 06/06/2022.
//

#ifndef SERVER_SOCKS_CONNECTION_H
#define SERVER_SOCKS_CONNECTION_H

#include "../tcp.h"
#include "../parsers/parser_types.h"

typedef enum {
    SOCKS5_CS_INIT,
    SOCKS5_CS_AUTH,
    SOCKS5_CS_READY,
    SOCKS5_CS_FINISHED
}SOCKS5_CONNECTION_STATUS;

typedef struct {
    SOCKS5_CONNECTION_STATUS Status;
    TcpSocket * Socket;
    void * Parser;
    ParserType ParserType;
} Socks5Connection;


Socks5Connection *Socks5ConnectionInit(TcpSocket * tcpSocket);
void Socks5ConnectionDestroy(Socks5Connection * connection);
bool RunSocks5(Socks5Connection * connection, byte * data, int length);

#endif //SERVER_SOCKS_CONNECTION_H
