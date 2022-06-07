//
// Created by Lucas Dell'Isola on 30/05/2022.
//

#ifndef SERVER_SOCKS_SERVER_H
#define SERVER_SOCKS_SERVER_H
#include "tcp/tcp.h"
#include "socks5_connection.h"


TcpSocket * InitSocks5Server(const char * port);

Socks5Connection * WaitForNewSocks5Connections(TcpSocket * server);


#endif //SERVER_SOCKS_SERVER_H
