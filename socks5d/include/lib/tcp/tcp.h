//
// Created by Lucas Dell'Isola on 28/05/2022.
//

#ifndef SERVER_TCP_H
#define SERVER_TCP_H


#include <stdbool.h>
#include <sys/socket.h>
#include "utils/utils.h"
#include <netdb.h>


typedef union {
    struct sockaddr_in6 ipv6;
    struct sockaddr_in ipv4;
    struct sockaddr base;
} TcpSocketAddress;

typedef struct {
    int FileDescriptor;
    int AddressFamily;
    TcpSocketAddress AddressInfo;
    bool IsPassive;
}TcpSocket;


/**
 * It initializes a TCP server
 * @param port Port where the server will be listening to, in the form of a null terminated string
 * @return a new Tcp Socket
 */
TcpSocket  * InitTcpServer(const char * port);

/**
 * It blocks the main thread while waiting for a new tcp connection
 * @param server Tcp socket where the server is listening to
 * @return A new tcp socket
 */
TcpSocket * WaitForNewConnections(TcpSocket * tcpServer);

/**
 * It safely disposes a tcp socket
 * @param socket socket to be disposed
 * @return OK or ERROR whether there were any errors.
 */
int DisposeTcpSocket(TcpSocket * socket);

/**
 * It reads content from a TCP socket
 * @param socket Tcp socket to read from
 * @param buffer Buffer array to store the content
 * @param bufferLength Max size of the buffer
 * @return The size of the content read
 */
size_t ReadFromTcpSocket(TcpSocket * socket, byte * buffer, int bufferLength);

/**
 * It writes data to a TCP socket
 * @param socket Tcp socket to write to
 * @param content Content to write to the socket
 * @param contentLength Content length to write
 * @return The size of the content written
 */
size_t WriteToTcpSocket(TcpSocket * socket, byte * content, int contentLength);

/**
 * It disconnects from a Tcp socket but it does not dispose it
 * @param socket
 * @return
 */
int DisconnectFromTcpSocket(TcpSocket * socket);


#endif //SERVER_TCP_H
