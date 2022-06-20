//
// Created by Lucas Dell'Isola on 28/05/2022.
//

#include <memory.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include "utils/utils.h"
#include "utils/logger.h"
#include "selector/selector.h"
#include "tcp/tcp.h"


static int ListenOnTcp(unsigned int port, const FdHandler *handler, struct sockaddr *address, socklen_t addressSize,
                int concurrentConnections);



static fd_selector selector = NULL;
static bool isRunning = true;

static const SelectorOptions options = {
        .Signal = SIGALRM,
        .SelectTimeout = {
                .tv_sec = 10,
                .tv_nsec = 0,
        }
};

bool InitTcpServer(const SelectorOptions *optionalOptions, int poolSize) {

    CreateTcpConnectionPool(poolSize);

    if (SELECTOR_STATUS_SUCCESS != SelectorInit(null == optionalOptions ? &options : optionalOptions)) {
        Error( "Cannot initialize Selector");
        return false;
    }

    selector = SelectorNew(1024);
    if (null == selector) {
        Error( "Cannot create selector");
        return false;
    }

    return true;
}


int IPv4ListenOnTcpPort(unsigned int port, const FdHandler *handler, const char *address, unsigned concurrentConnections) {
    Debug("Staring TCP server on IPv4...");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if ( null != address) {
        struct in_addr in4Addr;

        switch (inet_pton(AF_INET, address, &in4Addr)) {
            case 1:
                LogDebug("IPv4 address %s parsed successfully", address);
                break;
            case 0:
                LogErrorWithReason("Address %s cannot be parsed into an IPv4", address);
                return FUNCTION_ERROR;
            default:
                ErrorWithReason("Unknown error parsing IPv4");
                return FUNCTION_ERROR;
        }

        addr.sin_addr = in4Addr;
    }

    return ListenOnTcp(port, handler, (struct sockaddr *) &addr, sizeof(addr), concurrentConnections);
}

int IPv6ListenOnTcpPort(unsigned int port, const FdHandler *handler, const char *address, unsigned concurrentConnections) {
    Debug("Staring TCP server on IPv6...");

    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_addr = in6addr_any;
    addr.sin6_port = htons(port);

    if (null != address) {
        struct in6_addr in6Addr;
        switch (inet_pton(AF_INET6, address, &in6Addr)) {
            case 1:
                LogDebug("IPv6 address %s parsed successfully", address);
                break;
            case 0:
                LogErrorWithReason("Address %s cannot be parsed into an IPv6", address);
                return FUNCTION_ERROR;
            default:
                ErrorWithReason("Unknown error parsing IPv6");
                return FUNCTION_ERROR;
        }
        addr.sin6_addr = in6Addr;
    }

    return ListenOnTcp(port, handler, (struct sockaddr *) &addr, sizeof(addr), concurrentConnections);

}

bool RunTcpServer() {
    SelectorStatus selectorStatus;
    while (isRunning) {
        selectorStatus = SelectorSelect((fd_selector) selector);
        if (selectorStatus != SELECTOR_STATUS_SUCCESS) {
            Error("Error on selector. Exiting...");
            return false;
        }
    }
    return true;
}

void StopTcpServer() {
    isRunning = false;
}

void DisposeTcpServer() {
    SelectorClose();
    SelectorDestroy(selector);
    CleanTcpConnectionPool();

}


TcpConnection *AcceptNewTcpConnection(int fd) {
    Debug("Waiting for new TCP connections...");
    struct sockaddr_storage clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    const int client = accept(fd, (struct sockaddr *) &clientAddr, &clientAddrLen);

    if (-1 == client) {
        LogError("Cannot accept client connection in fd %d", fd);
        return null;
    }

    if (-1 == SelectorFdSetNio(client)) {
        LogError("Cannot set client socket non-blocking in fd %d", client);
        return null;
    }

    TcpConnection *tcpConnection = CreateTcpConnection(client, &clientAddr, clientAddrLen);

    LogDebug("New TCP connection up and running on file descriptor %d", tcpConnection->FileDescriptor);

    return tcpConnection;
}


ssize_t ReadFromTcpConnection(TcpConnection *socket, byte *buffer, size_t bufferLength) {
    if (socket == null) {
        Error("Cannot ReadHead from null TCP socket");
        return FUNCTION_ERROR;
    }

    if (buffer == null) {
        Error("TCP ReadHead buffer cannot be null");
        return FUNCTION_ERROR;
    }

    LogDebug("Reading from TCP socket on file descriptor %d...", socket->FileDescriptor);
    long bytes = recv(socket->FileDescriptor, buffer, bufferLength, 0);

    if (bytes < 0) {
        LogErrorWithReason("Could not read from TCP socket on file descriptor %d", socket->FileDescriptor);
        return FUNCTION_ERROR;
    }

    LogDebug("Read %d bytes from TCP socket on file descriptor %d", bytes, socket->FileDescriptor);
    return bytes;
}

ssize_t WriteToTcpConnection(TcpConnection *socket, byte *content, size_t contentLength) {
    if (socket == null) {
        Error("Cannot WriteHead to null TCP socket");
        return FUNCTION_ERROR;
    }

    if (content == null) {
        Error("TCP WriteHead buffer cannot be null");
        return FUNCTION_ERROR;
    }

    LogDebug("Writing %d bytes to TCP socket on file descriptor %d...", contentLength, socket->FileDescriptor);

    long bytes = send(socket->FileDescriptor, content, contentLength, 0);

    if (bytes < 0) {
        LogErrorWithReason("Could not WriteHead to TCP socket on file descriptor %d", socket->FileDescriptor);
        return FUNCTION_ERROR;
    }

    LogDebug("Wrote %d bytes to TCP socket on file descriptor %d", bytes, socket->FileDescriptor);

    return bytes;
}

TcpConnection *ConnectToIPv4TcpServer(struct sockaddr * address, const FdHandler *handler, void *data) {

    if (null == address) {
        Error("Address cannot be null");
        return null;
    }

    if (AF_INET != address->sa_family){
        Error("Only IPv4 addresses are supported");
        return null;
    }

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));

    if (-1 == SelectorFdSetNio(sock)) {
        Error("Cannot set server socket flags");
        close(sock);
        return false;
    }

    if (null == selector) {
        Error("Selector not created!");
        close(sock);
        return false;
    }

    SelectorStatus status = SelectorRegister(
            selector,
            sock,
            handler,
            SELECTOR_OP_WRITE | SELECTOR_OP_READ,
            data
    );

    if (status != SELECTOR_STATUS_SUCCESS) {
        Error("Cannot register TCP socket on selector");
        close(sock);
        return null;
    }


    int result = connect(sock, address, INET_ADDRSTRLEN);

    if (EINPROGRESS == errno || 0 == result) {
        return CreateTcpConnection(sock, (struct sockaddr_storage *) address, sizeof(*address));
    }

    char buffer[INET_ADDRSTRLEN +1];
    GetIPFromAddress((struct sockaddr_storage *) address, buffer, INET_ADDRSTRLEN + 1);
    LogWarningWithReason("Connection to IPv4 address %s failed", buffer);

    return null;
}

TcpConnection *ConnectToIPv6TcpServer(struct sockaddr * address, const FdHandler *handler, void *data) {
    Debug("Connecting to IPv6 server...");

    if (null == address){
        Error("Address cannot be null");
        return null;
    }

    if (AF_INET6 != address->sa_family){
        Error("Only IPv6 addresses are supported");
        return null;
    }


    int sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

    LogDebug("Remote IPv6 connection with file descriptor %d",sock);

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));

    if (-1 == SelectorFdSetNio(sock)) {
        Error("Cannot set server socket flags");
        close(sock);
        return false;
    }

    LogDebug("Sock with file descriptor %d is non-blocking",sock);

    if (null == selector) {
        Error("Selector not created!");
        close(sock);
        return false;
    }


    int result = connect(sock, address, INET6_ADDRSTRLEN);

    if (EINPROGRESS == errno || 0 == result) {
        SelectorStatus status = SelectorRegister(
                selector,
                sock,
                handler,
                SELECTOR_OP_WRITE | SELECTOR_OP_READ,
                data
        );

        if (status != SELECTOR_STATUS_SUCCESS) {
            Error("Cannot register TCP socket on selector");
            close(sock);
            return null;
        }
        Debug("Remote IPv6 connection succeeded!");
        return CreateTcpConnection(sock, (struct sockaddr_storage *) address, sizeof(*address));
    }

    char buffer[INET6_ADDRSTRLEN +1];
    GetIPFromAddress((struct sockaddr_storage *) address, buffer, INET6_ADDRSTRLEN + 1);
    LogWarningWithReason("Connection to IPv6 address %s failed", buffer);

    return null;
}

fd_selector GetSelector() {
    return selector;
}


int ListenOnTcp(unsigned int port, const FdHandler *handler, struct sockaddr *address, socklen_t addressSize,
                int concurrentConnections) {

    if (port > 65535 || port <= 0) {
        Error("Invalid port. Cannot be null");
        return FUNCTION_ERROR;
    }

    if (null == address){
        Error("Address cannot be null");
        return FUNCTION_ERROR;
    }

    if (null == selector) {
        Error("Selector not created!");
        return FUNCTION_ERROR;
    }

    int servSock = socket(address->sa_family, SOCK_STREAM, IPPROTO_TCP);

    if (servSock < 0) {
        ErrorWithReason("Cannot open passive socket");
        return FUNCTION_ERROR;
    }

    setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));

    if (AF_INET6 == address->sa_family)
        setsockopt(servSock, SOL_IPV6, IPV6_V6ONLY, &(int) {1}, sizeof(int));

    LogDebug("Opened passive socket %d for TCP server",servSock);

    if (bind(servSock, address, addressSize) < 0) {
        ErrorWithReason("Cannot bind socket to file descriptor");
        close(servSock);
        return FUNCTION_ERROR;
    }

    Debug("Bound TCP server socket to file descriptor");

    if (listen(servSock, concurrentConnections) < 0) {
        ErrorWithReason("Cannot set passive socket to listen");
        close(servSock);
        return FUNCTION_ERROR;
    }

    LogDebug("Set TCP server socket %d to listen", servSock);

    if (-1 == SelectorFdSetNio(servSock)) {
        Error("Cannot set server socket flags");
        close(servSock);
        return FUNCTION_ERROR;
    }

    SelectorStatus status = SelectorRegister((fd_selector) selector, servSock, handler, SELECTOR_OP_READ, null);

    if (status != SELECTOR_STATUS_SUCCESS) {
        Error("Cannot register TCP socket on selector");
        close(servSock);
        return FUNCTION_ERROR;
    }

    return servSock;

}



