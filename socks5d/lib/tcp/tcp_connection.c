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
#include "utils/object_pool.h"
#include "tcp/tcp.h"


static void destroy(void * obj){
    TcpConnection * connection = (TcpConnection*) obj;
    memset(connection,0, sizeof(TcpConnection));
    connection->FileDescriptor = -1;
}

static ObjectPool tcpPool;
static ObjectPoolHandlers handlers = {
    .OnCreate = null,
    .OnDispose = destroy
};

int DisposeTcpConnection(TcpConnection *tcpConnection, fd_selector selector) {
    if (tcpConnection == null) {
        Warning("Failed to dispose TCP tcpConnection. Cannot be null");
        return FUNCTION_ERROR;
    }

    LogDebug("Disposing TCP tcpConnection on file descriptor %d", tcpConnection->FileDescriptor);

    if (SELECTOR_STATUS_SUCCESS != SelectorUnregisterFd(selector, tcpConnection->FileDescriptor)) {
        Warning("Cannot unregister file descriptor");
    }

    if (close(tcpConnection->FileDescriptor) < 0) {
        LogWarningWithReason("Cannot close file descriptor %d", tcpConnection->FileDescriptor);
        return FUNCTION_ERROR;
    }

    LogDebug("File descriptor %d closed successfully", tcpConnection->FileDescriptor);

    DestroyObject(&tcpPool, tcpConnection);

    Debug("TCP tcpConnection disposed successfully");

    return FUNCTION_OK;
}

int DisconnectFromTcpConnection(TcpConnection *socket, int how) {
    if (socket == null) {
        Warning( "Failed to disconnect TCP socket. Cannot be null");
        return FUNCTION_ERROR;
    }

    LogDebug("Disconnecting %s from TCP socket on file descriptor %d...",
             GetShutdownModeName(how),
            socket->FileDescriptor
    );

    int shutdownResult = shutdown(socket->FileDescriptor, how);

    if (shutdownResult < 0 && (errno != ENOTCONN || socket->CanWrite)) {
        LogWarningWithReason("Cannot close TCP socket on file descriptor %d...", socket->FileDescriptor);
        return FUNCTION_ERROR;
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
        default:
            break;
    }


    LogDebug("Successfully disconnected %s from TCP socket on file descriptor %d!", GetShutdownModeName(how),
            socket->FileDescriptor);

    return FUNCTION_OK;
}

TcpConnection *CreateTcpConnection(int fd, struct sockaddr_storage *addr, socklen_t addrSize) {

    TcpConnection *tcpSocket = GetObjectFromPool(&tcpPool);

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
        Warning("Tcp connection cannot be null");
        return false;
    }

    return !connection->CanWrite && !connection->CanRead;
}

bool IsTcpConnectionReady(TcpConnection *connection) {
    int error = 0;
    socklen_t len = sizeof(error);
    int result = getsockopt(connection->FileDescriptor, SOL_SOCKET, SO_ERROR, &error, &len);

    if (result < 0) {
        Warning("Cannot get Socket Options");
        return false;
    }

    if (error != 0) {
        errno = error;
        return false;
    }

    return true;
}

static void DisposeWrapper(void * con, void * data){
    DisposeTcpConnection(con,data);
}

void CleanTcpConnectionPool() {
    Debug("Clearing TCP socks5Pool");
    ExecuteOnExistingElements(&tcpPool, DisposeWrapper , GetSelector());
    CleanObjectPool(&tcpPool);
}



void CreateTcpConnectionPool(int initialSize) {
   Debug("Initializing TCP Pool");
   InitObjectPool(&tcpPool, &handlers, initialSize, sizeof(TcpConnection));
}


















