//
// Created by Lucas Dell'Isola on 09/06/2022.
//

#include <unistd.h>
#include <memory.h>
#include "tcp/tcp_connection.h"
#include "utils/logger.h"
#include "utils/utils.h"
#include "selector/selector.h"

int DisposeTcpConnection(TcpConnection *socket, fd_selector selector) {
    if (socket == null) {
        LogError(false,"Failed to dispose TCP socket. Cannot be null");
        return ERROR;
    }

    LogInfo("Disposing TCP socket on file descriptor %d",socket->FileDescriptor);

    if ( SELECTOR_STATUS_SUCCESS != SelectorUnregisterFd(selector,socket->FileDescriptor)){
        LogError(false,"Cannot unregister file descriptor");
    }

    if (close(socket->FileDescriptor) < 0){
        LogError(true,"Cannot close file descriptor %d",socket->FileDescriptor);
        return ERROR;
    }

    LogInfo("File descriptor %d closed successfully");

    free(socket);

    LogInfo("TCP socket disposed successfully");

    return OK;
}

int DisconnectFromTcpConnection(TcpConnection * socket, int how){
    if (socket == null)
    {
        LogError(false,"Failed to disconnect TCP socket. Cannot be null");
        return ERROR;
    }

    LogInfo("Disconnecting from TCP socket on file descriptor %d...",socket->FileDescriptor);

    if (shutdown(socket->FileDescriptor,how) < 0){
        LogError(true,"Cannot close TCP socket on file descriptor %d...",socket->FileDescriptor);
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


    LogInfo("Successfully disconnected from TCP socket on file descriptor %d!",socket->FileDescriptor);

    return OK;
}

TcpConnection *CreateTcpConnection(int fd, struct sockaddr_storage * addr, socklen_t addrSize) {
    TcpConnection * tcpSocket = calloc(1, sizeof(TcpConnection));

    memcpy(&tcpSocket->Address, addr, addrSize);
    tcpSocket->AddressLength = addrSize;
    tcpSocket->FileDescriptor = fd;
    tcpSocket->CanRead = true;
    tcpSocket->CanWrite = true;

    return tcpSocket;
}

bool IsTcpConnectionDisconnected(TcpConnection *connection) {
    if (null == connection)
    {
        LogError(false,"Tcp connection cannot be null");
    }

    return !connection->CanWrite && !connection->CanRead;
}
