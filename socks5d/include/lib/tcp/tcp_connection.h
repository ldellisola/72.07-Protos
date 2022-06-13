//
// Created by Lucas Dell'Isola on 09/06/2022.
//


#ifndef SOCKS5D_TCP_CONNECTION_H
#define SOCKS5D_TCP_CONNECTION_H

#include <netinet/in.h>
#include "selector/selector.h"



typedef struct {
    int FileDescriptor;
    struct sockaddr_storage Address;
    socklen_t AddressLength;
}TcpConnection;

/**
 * It returns a new instance of a TCP connection
 * @param fd file descriptor of the TPC connection
 * @param addr address of the client
 * @param addrSize size of the address of the client
 * @return a new Tcp connection
 */
TcpConnection * CreateTcpConnection(int fd, struct sockaddr_storage * addr, socklen_t addrSize);

/**
 * It safely disposes a tcp socket
 * @param socket socket to be disposed
 * @return OK or ERROR whether there were any errors.
 */
int DisposeTcpConnection(TcpConnection *socket, fd_selector selector);

/**
 * It disconnects from a Tcp socket but it does not dispose it
 * @param socket
 * @return
 */
int DisconnectFromTcpConnection(TcpConnection * socket);


#endif //SOCKS5D_TCP_CONNECTION_H



