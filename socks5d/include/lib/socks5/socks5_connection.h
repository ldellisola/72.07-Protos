//
// Created by Lucas Dell'Isola on 06/06/2022.
//

#ifndef SERVER_SOCKS_CONNECTION_H
#define SERVER_SOCKS_CONNECTION_H

#include "tcp/tcp.h"
#include "socks5/fsm_handlers/socks5_hello.h"
#include "utils/buffer.h"
#include "socks5_connection_status.h"
#include "fsm/fsm.h"
#include "socks5/fsm_handlers/socks5_auth.h"
#include "socks5/fsm_handlers/socks5_request.h"
#include "socks5/fsm_handlers/socks5_establish_connection.h"
#include "parsers/pop3/pop3_auth_parser.h"
#include "socks5_users.h"
#include "utils/object_pool.h"

typedef struct {
    time_t LastConnectionOn;
    FiniteStateMachine Fsm;
    TcpConnection *ClientTcpConnection;
    TcpConnection *RemoteTcpConnection;
    // Remote address and port are not guaranteed to exist anywhere else
    char * RemoteAddressString;
    in_port_t RemotePort;

    struct Socks5User * User;
    const FdHandler *Handler;
    union {
        HelloData Hello;
        AuthData Auth;
        RequestData Request;
        Pop3AuthParser Pop3Parser;
    } Data;
    ArrayBuffer ReadBuffer, WriteBuffer;
} Socks5Connection;

/**
* It creates a Socks5 tcpConnection from a given TCP tcpConnection
 * @param tcpConnection File descriptor for the socks tcpConnection
*/
Socks5Connection *CreateSocks5Connection(TcpConnection *tcpConnection);

/**
 * It safely disposes a Socks5 connection
 * @param connection Socks5 connection to dispose
 */
void DisposeSocks5Connection(Socks5Connection *connection, fd_selector selector);

/**
 * It initializes the socks5 connection socks5Pool with a given size preallocated
 * @param initialSize initial size of the socks5Pool
 */
void CreateSocks5ConnectionPool(int initialSize);

/**
 * It disposes all Socks5 connections
 */
void CleanSocks5ConnectionPool();

/**
 * It returns a pointer to the socks5 connection pool
 * @return Pointer to the socks5connnection pool.
 */
ObjectPool * GetSocks5ConnectionPool();



#endif //SERVER_SOCKS_CONNECTION_H
