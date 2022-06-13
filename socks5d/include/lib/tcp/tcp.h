//
// Created by Lucas Dell'Isola on 28/05/2022.
//

#ifndef SERVER_TCP_H
#define SERVER_TCP_H


#include <stdbool.h>
#include <sys/socket.h>
#include "utils/utils.h"
#include "selector/selector.h"
#include "tcp/tcp_connection.h"
#include <netdb.h>

fd_selector GetSelect();

/**
 * It initializes a TCP server and creates the internal selector
 * @param optionalOptions Optional parameter to define your own Selector options. If null it will use a default configuration
 */
void InitTcpServer(const SelectorOptions * optionalOptions);

/**
 * It sets up a listener socket on a given port, using an IPv4 address
 * @param port Port where the server will be listening to, in the form of a null terminated string
 * @param handler A set of callbacks to execute on ReadHead, WriteHead and close
 * @return Whether the operation was a success or not
 */
bool IPv4ListenOnTcpPort(unsigned int port, const FdHandler *handler);

/**
 * It runs the TCP server until it's stopped
 * @return Whether there was an error or not.
 */
bool RunTcpServer();

/**
 * It stops the TCP server
 */
void StopTcpServer();

/**
 * It blocks the main thread while waiting for a new tcp connection
 * @param fd file descriptor to listen to
 * @return A new tcp socket
 */
TcpConnection * AcceptNewTcpConnection(int fd);

/**
 * It reads content from a TCP socket
 * @param socket Tcp socket to ReadHead from
 * @param buffer Buffer array to store the content
 * @param bufferLength Max size of the buffer
 * @return The size of the content ReadHead
 */
ssize_t ReadFromTcpConnection(TcpConnection * socket, byte * buffer, size_t bufferLength);

/**
 * It writes Data to a TCP socket
 * @param socket Tcp socket to WriteHead to
 * @param content Content to WriteHead to the socket
 * @param contentLength Content length to WriteHead
 * @return The size of the content written
 */
size_t WriteToTcpConnection(TcpConnection * socket, byte * content, size_t contentLength);




#endif //SERVER_TCP_H
