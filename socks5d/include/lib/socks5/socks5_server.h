//
// Created by Lucas Dell'Isola on 30/05/2022.
//

#ifndef SERVER_SOCKS_SERVER_H
#define SERVER_SOCKS_SERVER_H
#include "tcp/tcp.h"
#include "socks5_connection.h"

/**
 * It initializes a Socks5Server
 * @param port Port where the server will be listening to, in the form of a null terminated string
 * @return A TCP Socket
 */
TcpSocket * InitSocks5Server(const char * port);

/**
 * It blocks the main thread while waiting for a new Socks5 connection
 * @param server Tcp socket where the server is listening to
 * @return A new Socks5 connection
 */
Socks5Connection * WaitForNewSocks5Connections(TcpSocket * server);


#endif //SERVER_SOCKS_SERVER_H
