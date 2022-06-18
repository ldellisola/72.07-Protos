//
// Created by Lucas Dell'Isola on 09/06/2022.
//
#include "tcp/tcp_connection.h"
#include <unistd.h>
#include <memory.h>
#include <errno.h>
#include <assert.h>
#include "utils/logger.h"
#include "utils/utils.h"
#include "selector/selector.h"

typedef enum {
    PooledTcpConnectionEmpty,
    PooledTcpConnectionInUse
} PooledTcpConnectionStatus;


typedef struct PooledTcpConnection_ {
    TcpConnection Connection;
    PooledTcpConnectionStatus Status;
    struct PooledTcpConnection_ *Next;
} PooledTcpConnection;

PooledTcpConnection *tcpPool = null;

TcpConnection * GetTcpConnection();
void DestroyTcpConnection(TcpConnection * connection);


int DisposeTcpConnection(TcpConnection *tcpConnection, fd_selector selector) {
    if (tcpConnection == null) {
        LogError(false, "Failed to dispose TCP tcpConnection. Cannot be null");
        return ERROR;
    }

    LogInfo("Disposing TCP tcpConnection on file descriptor %d", tcpConnection->FileDescriptor);

    if (SELECTOR_STATUS_SUCCESS != SelectorUnregisterFd(selector, tcpConnection->FileDescriptor)) {
        LogError(false, "Cannot unregister file descriptor");
    }

    if (close(tcpConnection->FileDescriptor) < 0) {
        LogError(true, "Cannot close file descriptor %d", tcpConnection->FileDescriptor);
        return ERROR;
    }

    LogInfo("File descriptor %d closed successfully", tcpConnection->FileDescriptor);

    DestroyTcpConnection(tcpConnection);

    LogInfo("TCP tcpConnection disposed successfully");

    return OK;
}

int DisconnectFromTcpConnection(TcpConnection *socket, int how) {
    if (socket == null) {
        LogError(false, "Failed to disconnect TCP socket. Cannot be null");
        return ERROR;
    }

    LogInfo("Disconnecting %s from TCP socket on file descriptor %d...", GetShutdownModeName(how),
            socket->FileDescriptor);

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


    LogInfo("Successfully disconnected %s from TCP socket on file descriptor %d!", GetShutdownModeName(how),
            socket->FileDescriptor);

    return OK;
}

TcpConnection *CreateTcpConnection(int fd, struct sockaddr_storage *addr, socklen_t addrSize) {

    TcpConnection *tcpSocket = GetTcpConnection();

    memcpy(&tcpSocket->Address, addr, addrSize);
    tcpSocket->AddressLength = addrSize;
    tcpSocket->FileDescriptor = fd;
    tcpSocket->CanRead = true;
    tcpSocket->CanWrite = true;

    GetIPFromAddress(addr,tcpSocket->AddressString, sizeof(tcpSocket->AddressString));
    tcpSocket->Port = GetPortFromAddress(addr);

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
    socklen_t len = sizeof(error);
    int result = getsockopt(connection->FileDescriptor, SOL_SOCKET, SO_ERROR, &error, &len);

    if (result < 0) {
        LogError(false, "Cannot get Socket Options");
        return false;
    }

    if (error != 0) {
        errno = error;
        return false;
    }


    return true;
}



void DestroyTcpConnection(TcpConnection * connection){
    if (null == tcpPool) {
        LogError(false, "TCP pool was not initialized");
        return;
    }

    memset(connection,0, sizeof(TcpConnection));
    connection->FileDescriptor = -1;

    PooledTcpConnection * temp;

    for (temp = tcpPool; temp != null && &temp->Connection != connection ; temp = temp->Next);

    if (null == temp)
    {
        LogError(false,"Error while destroying connection!");
        return;
    }
    assert(&temp->Connection == connection);
    temp->Status = PooledTcpConnectionEmpty;
}


TcpConnection *GetTcpConnection() {
    if (null == tcpPool) {
        LogError(false, "TCP pool was not initialized");
        return null;
    }

    PooledTcpConnection * temp;

    for ( temp = tcpPool; temp != null ; temp = temp->Next) {
        if (PooledTcpConnectionEmpty == temp->Status) {
            temp->Status = PooledTcpConnectionInUse;
            return &temp->Connection;
        }

        if (null == temp->Next)
            break;
    }

    temp->Next = calloc(1, sizeof(PooledTcpConnection));
    temp = temp->Next;
    temp->Status = PooledTcpConnectionInUse;

    return &temp->Connection;

}


void CleanTcpConnectionPool() {
    LogInfo("Clening Tcp connection pool");
    if (null == tcpPool) {
        LogError(false, "TCP pool was not initialized. Cannot clean it");
        return;
    }

    PooledTcpConnection * next;
    for (PooledTcpConnection * conn = tcpPool; conn != null ; conn = next) {
        next = conn->Next;
        if (PooledTcpConnectionInUse == conn->Status){
            close(conn->Connection.FileDescriptor);
        }
        free(conn);
    }
}

void CreateTcpConnectionPool(int initialSize) {
    LogInfo("Initializing TCP Pool");
    if (initialSize < 1) {
        LogInfo("Invalid initial pool size %d, using default value 1", initialSize);
        initialSize = 1;
    }

    tcpPool = calloc(1, sizeof(PooledTcpConnection));
    tcpPool->Status = PooledTcpConnectionEmpty;
    PooledTcpConnection *current = tcpPool;

    while (--initialSize > 0) {
        current->Next = calloc(1, sizeof(PooledTcpConnection));
        current = current->Next;
        current->Status = PooledTcpConnectionEmpty;
    }

}


















