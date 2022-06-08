//
// Created by Lucas Dell'Isola on 06/06/2022.
//

#ifndef SERVER_SOCKS_CONNECTION_H
#define SERVER_SOCKS_CONNECTION_H

#include "tcp/tcp.h"
#include "../parsers/parser_types.h"

typedef enum {
    SOCKS5_CS_INIT,
    SOCKS5_CS_AUTH,
    SOCKS5_CS_REQUEST,
    SOCKS5_CS_READY,
    SOCKS5_CS_FINISHED,
    SOCKS5_CS_FAILED
}SOCKS5_CONNECTION_STATUS;

typedef struct {
    SOCKS5_CONNECTION_STATUS Status;
    TcpSocket * Socket;
    void * Parser;
    ParserType ParserType;
} Socks5Connection;

/**
* It creates a Socks5 connection from a given TCP connection
* @param tcpSocket  Active TCP socket connected to the client
*/
Socks5Connection *Socks5ConnectionInit(TcpSocket * tcpSocket);

/**
 * It safely disposes a Socks5 connection
 * @param connection Socks5 connection to dispose
 */
void Socks5ConnectionDestroy(Socks5Connection * connection);

/**
 * It runs through the states of a Socks5 finite state machine
 * @param connection Socks5 connection
 * @param data Input data to consume
 * @param length Size of the data to consume
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
