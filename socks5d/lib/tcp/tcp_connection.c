//
// Created by Lucas Dell'Isola on 09/06/2022.
//
#include "tcp/tcp_connection.h"
#include <unistd.h>
#include <memory.h>
#include <errno.h>
#include "utils/logger.h"
#include "utils/utils.h"
#include "selector/selector.h"

int DisposeTcpConnection(TcpConnection *socket, fd_selector selector) {
    if (socket == null) {
        LogError(false, "Failed to dispose TCP socket. Cannot be null");
        return ERROR;
    }

    LogInfo("Disposing TCP socket on file descriptor %d", socket->FileDescriptor);

    if (SELECTOR_STATUS_SUCCESS != SelectorUnregisterFd(selector, socket->FileDescriptor)) {
        LogError(false, "Cannot unregister file descriptor");
    }

    if (close(socket->FileDescriptor) < 0) {
        LogError(true, "Cannot close file descriptor %d", socket->FileDescriptor);
        return ERROR;
    }

    LogInfo("File descriptor %d closed successfully",socket->FileDescriptor);

    free(socket);

    LogInfo("TCP socket disposed successfully");

    return OK;
}

int DisconnectFromTcpConnection(TcpConnection *socket, int how) {
    if (socket == null) {
        LogError(false, "Failed to disconnect TCP socket. Cannot be null");
        return ERROR;
    }

    LogInfo("Disconnecting %s from TCP socket on file descriptor %d...", GetShutdownModeName(how), socket->FileDescriptor);

    int shutdownResult = shutdown(socket->FileDescriptor, how);

    if (shutdownResult < 0 && (errno != ENOTCONN || socket->CanWrite)) {
        LogError(true, "Cannot close TCP socket on file descriptor %d...", socket->FileDescriptor);
        return ERROR;
    }

    switch (how) {
        case SHUT_RD:
            socket->CanRead = false;
            break;
        case SHUT_WR:
            socket->CanWrite = false;
            break;
        case SHUT_RDWR:
            socket->CanRead = false;
            socket->CanWrite = false;
            break;
    }


    LogInfo("Successfully disconnected %s from TCP socket on file descriptor %d!", GetShutdownModeName(how), socket->FileDescriptor);

    return OK;
}

TcpConnection *CreateTcpConnection(int fd, struct sockaddr_storage *addr, socklen_t addrSize) {
    TcpConnection *tcpSocket = calloc(1, sizeof(TcpConnection));

    memcpy(&tcpSocket->Address, addr, addrSize);
    tcpSocket->AddressLength = addrSize;
    tcpSocket->FileDescriptor = fd;
    tcpSocket->CanRead = true;
    tcpSocket->CanWrite = true;

    return tcpSocket;
}

bool IsTcpConnectionDisconnected(TcpConnection *connection) {
    if (null == connection) {
        LogError(false, "Tcp connection cannot be null");
    }

    return !connection->CanWrite && !connection->CanRead;
}

bool IsTcpConnectionReady(TcpConnection *connection) {
    int error = 0;
    socklen_t len = sizeof (error);
    int result = getsockopt(connection->FileDescriptor,SOL_SOCKET,SO_ERROR,&error,&len);

    if (result < 0)
    {
        LogError(false,"Cannot get Socket Options");
        return false;
    }

    if (error != 0){
        errno = error;
        return false;
    }


    return true;
}
