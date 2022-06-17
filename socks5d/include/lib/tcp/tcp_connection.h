//
// Created by Lucas Dell'Isola on 09/06/2022.
//


#ifndef SOCKS5D_TCP_CONNECTION_H
#define SOCKS5D_TCP_CONNECTION_H

#include <netinet/in.h>
#include <stdbool.h>
#include "selector/selector.h"

typedef struct {
    int FileDescriptor;
    struct sockaddr_storage Address;
    socklen_t AddressLength;
    bool CanWrite;
    bool CanRead;
} TcpConnection;

/**
 * It returns a new instance of a TCP connection
 * @param fd file descriptor of the TPC connection
 * @param addr address of the client
 * @param addrSize size of the address of the client
 * @return a new Tcp connection
 */
TcpConnection *CreateTcpConnection(int fd, struct sockaddr_storage *addr, socklen_t addrSize);

/**
 * It safely disposes a tcp tcpConnection
 * @param tcpConnection tcpConnection to be disposed
 * @return OK or ERROR whether there were any errors.
 */
int DisposeTcpConnection(TcpConnection *tcpConnection, fd_selector selector);

/**
 * It disconnects from a Tcp socket but it does not dispose it
 * @param socket
 * @param how
 * @return
 */
int DisconnectFromTcpConnection(TcpConnection *socket, int how);

/**
 * It says if the TCP Connection is is completely shut down (not read or write)
 * @param connection Tcp connection to evaluate
 * @return True if both read and write channels are closed
 */
bool IsTcpConnectionDisconnected(TcpConnection *connection);

/**
 * It checks if the Tcp connection is ready to be used. It will set the errno variable
 * with the appropiate accept error code if the request is not ready.
 * @param connection tcp connection to check
 * @return True if the connection is ready to be used
 */
bool IsTcpConnectionReady(TcpConnection * connection);

/**
 * It disposes all Tcp connections
 */
void CleanTcpConnectionPool();

/**
 * It initializes the TCP connection tcpPool with a given size
 * @param initialSize initial size of the tcp tcpPool
 */
void InitTcpConnectionPool(int initialSize);


#endif //SOCKS5D_TCP_CONNECTION_H



