//
// Created by Lucas Dell'Isola on 06/06/2022.
//

#ifndef SERVER_SOCKS_CONNECTION_H
#define SERVER_SOCKS_CONNECTION_H

#include "tcp/tcp.h"
#include "socks5/socks5_hello.h"
#include "utils/buffer.h"
#include "socks5_connection_status.h"
#include "fsm/fsm.h"
#include "socks5_auth.h"


typedef struct {
    FiniteStateMachine Fsm;
    CONNECTION_STATE State;
    TcpConnection * ClientTcpConnection;
    TcpConnection * RemoteTcpConnection;
    FdHandler * Handler;
    union {
        HelloData Hello;
        AuthData Auth;
    } Data;
    ArrayBuffer ReadBuffer, WriteBuffer;
} Socks5Connection;

/**
* It creates a Socks5 tcpConnection from a given TCP tcpConnection
 * @param tcpConnection File descriptor for the socks tcpConnection
*/
Socks5Connection *Socks5ConnectionInit(TcpConnection *tcpConnection);

/**
 * It safely disposes a Socks5 connection
 * @param connection Socks5 connection to dispose
 */
void Socks5ConnectionDestroy(Socks5Connection *connection, fd_selector selector);

/**
 * It runs through the States of a Socks5 finite state machine
 * @param connection Socks5 connection
 * @param data Input Data to consume
 * @param length Size of the Data to consume
 * @return True if the machine reached a final state
 */
bool RunSocks5(Socks5Connection * connection, byte * data, int length);

/**
 * It decides if a Socks5 finite state machine reached an invalid state
 * @param connection Socks5 connection to evaluate
 * @return True if the FSM reached an invalid state
 */
bool Socks5ConnectionFailed(Socks5Connection * connection);


#endif //SERVER_SOCKS_CONNECTION_H
