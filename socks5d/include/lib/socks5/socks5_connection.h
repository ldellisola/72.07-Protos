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
#include "socks5_users.h"

typedef struct {
    time_t LastConnectionOn;
    FiniteStateMachine Fsm;
    TcpConnection *ClientTcpConnection;
    TcpConnection *RemoteTcpConnection;
    struct Socks5User * User;
    const FdHandler *Handler;
    union {
        HelloData Hello;
        AuthData Auth;
        RequestData Request;
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
 * It initializes the socks5 connection pool with a given size preallocated
 * @param initialSize initial size of the pool
 */
void CreateSocks5ConnectionPool(int initialSize);

/**
 * It disposes all Socks5 connections
 */
void CleanSocks5ConnectionPool();

/**
 * It will check all current active socks5 connections and see if they are currently timed out
 * @param fdSelector Selector
 */
void CheckForTimeoutInSocks5Connections(fd_selector fdSelector);

/**
 * It updates the LastConnectionOn field
 * @param data pointer to Socks5Connection
 */
void NotifySocks5ConnectionAccess(void * data);

/**
 * It sets the time it takes for a connection to timeout
 * @param timeout Time in seconds. If its 0 or negative, then connections will never timeout
 */
void SetConnectionTimeout(time_t timeout);



#endif //SERVER_SOCKS_CONNECTION_H
