//
// Created by Lucas Dell'Isola on 09/06/2022.
//

#include <unistd.h>
#include <memory.h>
#include "tcp/tcp_connection.h"
#include "utils/logger.h"
#include "utils/utils.h"

int DisposeTcpConnection(TcpConnection * socket){
    if (socket == null) {
        LogError(false,"Failed to dispose TCP socket. Cannot be null");
        return ERROR;
    }

    LogInfo("Disposing TCP socket on file descriptor %d",socket->FileDescriptor);

    if (close(socket->FileDescriptor) < 0){
        LogError(true,"Cannot close file descriptor %d",socket->FileDescriptor);
        return ERROR;
    }

    LogInfo("File descriptor %d closed successfully");

    free(socket);

    LogInfo("TCP socket disposed successfully");

    return OK;
}

int DisconnectFromTcpConnection(TcpConnection * socket){
    if (socket == null)
    {
        LogError(false,"Failed to disconnect TCP socket. Cannot be null");
        return ERROR;
    }

    LogInfo("Disconnecting from TCP socket on file descriptor %d...",socket->FileDescriptor);

    if (shutdown(socket->FileDescriptor,SHUT_RDWR) < 0){
        LogError(true,"Cannot close TCP socket on file descriptor %d...",socket->FileDescriptor);
        return ERROR;
    }

    LogInfo("Successfully disconnected from TCP socket on file descriptor %d!",socket->FileDescriptor);

    return OK;
}

TcpConnection *CreateTcpConnection(int fd, struct sockaddr_storage *addr, socklen_t addrSize) {
    TcpConnection * tcpSocket = calloc(1, sizeof(TcpConnection));

    memcpy(&tcpSocket->Address, &addr, addrSize);
    tcpSocket->AddressLength = addrSize;

    return tcpSocket;
}
