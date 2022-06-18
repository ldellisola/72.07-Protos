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


int ListenOnTcp(unsigned int port, const FdHandler *handler, struct sockaddr *address, socklen_t addressSize,
                 unsigned concurrentConnections);



static fd_selector selector = NULL;
static bool isRunning = true;

const SelectorOptions options = {
        .Signal = SIGALRM,
        .SelectTimeout = {
                .tv_sec = 10,
                .tv_nsec = 0,
        }
};

bool InitTcpServer(const SelectorOptions *optionalOptions, int poolSize) {

    CreateTcpConnectionPool(poolSize);

    if (SELECTOR_STATUS_SUCCESS != SelectorInit(null == optionalOptions ? &options : optionalOptions)) {
        LogError(false, "Cannot initialize Selector");
        return false;
    }

    selector = SelectorNew(1024);
    if (null == selector) {
        LogError(false, "Cannot create selector");
        return false;
    }

    return true;
}


int IPv4ListenOnTcpPort(unsigned int port, const FdHandler *handler, const char *address, unsigned concurrentConnections) {
    LogInfo("Staring TCP server on IPv4...");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if ( null != address) {
        struct in_addr in4Addr;

        switch (inet_pton(AF_INET, address, &in4Addr)) {
            case 1:
                LogInfo("IPv4 address %s parsed successfully", address);
                break;
            case 0:
                LogError(true, "Address %s cannot be parsed into an IPv4", address);
                return -1;
            default:
                LogError(true, "Unknown error parsing IPv4");
                return -1;
        }

        addr.sin_addr = in4Addr;
    }

    return ListenOnTcp(port, handler, (struct sockaddr *) &addr, sizeof(addr), concurrentConnections);
}

int IPv6ListenOnTcpPort(unsigned int port, const FdHandler *handler, const char *address, unsigned concurrentConnections) {
    LogInfo("Staring TCP server on IPv6...");

    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_addr = in6addr_any;
    addr.sin6_port = htons(port);

    if (null != address) {
        struct in6_addr in6Addr;
        switch (inet_pton(AF_INET6, address, &in6Addr)) {
            case 1:
                LogInfo("IPv6 address %s parsed successfully", address);
                break;
            case 0:
                LogError(true, "Address %s cannot be parsed into an IPv6", address);
                return -1;
            default:
                LogError(true, "Unknown error parsing IPv6");
                return -1;
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
            LogError(false, "Error on selector. Exiting...");
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
    LogInfo("Waiting for new TCP connections...");
    struct sockaddr_storage clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    const int client = accept(fd, (struct sockaddr *) &clientAddr, &clientAddrLen);

    if (-1 == client) {
        LogError(false, "Cannot accept client connection in fd %d", fd);
        return null;
    }

    if (-1 == SelectorFdSetNio(client)) {
        LogError(false, "Cannot set client socket non-blocking in fd %d", client);
        return null;
    }

    TcpConnection *tcpConnection = CreateTcpConnection(client, &clientAddr, clientAddrLen);

    LogInfo("New TCP connection up and running on file descriptor %d", tcpConnection->FileDescriptor);

    return tcpConnection;
}


ssize_t ReadFromTcpConnection(TcpConnection *socket, byte *buffer, size_t bufferLength) {
    if (socket == null) {
        LogError(false, "Cannot ReadHead from null TCP socket");
        return ERROR;
    }

    if (buffer == null) {
        LogError(false, "TCP ReadHead buffer cannot be null");
        return ERROR;
    }

    LogInfo("Reading from TCP socket on file descriptor %d...", socket->FileDescriptor);
    long bytes = recv(socket->FileDescriptor, buffer, bufferLength, 0);

    if (bytes < 0) {
        LogError(true, "Could not ReadHead from TCP socket on file descriptor %d", socket->FileDescriptor);
        return ERROR;
    }

    LogInfo("Read %d bytes from TCP socket on file descriptor %d", bytes, socket->FileDescriptor);
    return bytes;
}

size_t WriteToTcpConnection(TcpConnection *socket, byte *content, size_t contentLength) {
    if (socket == null) {
        LogError(false, "Cannot WriteHead to null TCP socket");
        return ERROR;
    }

    if (content == null) {
        LogError(false, "TCP WriteHead buffer cannot be null");
        return ERROR;
    }

    LogInfo("Writing %d bytes to TCP socket on file descriptor %d...", contentLength, socket->FileDescriptor);

    long bytes = send(socket->FileDescriptor, content, contentLength, 0);

    if (bytes < 0) {
        LogError(true, "Could not WriteHead to TCP socket on file descriptor %d", socket->FileDescriptor);
        return ERROR;
    }

    LogInfo("Wrote %d bytes to TCP socket on file descriptor %d", bytes, socket->FileDescriptor);

    return bytes;
}

TcpConnection *ConnectToIPv4TcpServer(struct sockaddr * address, const FdHandler *handler, void *data) {

    if (null == address) {
        LogError(false, "Address cannot be null");
        return null;
    }

    if (AF_INET != address->sa_family){
        LogError(false,"Only IPv4 addresses are supported");
        return null;
    }

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));

    if (-1 == SelectorFdSetNio(sock)) {
        LogError(false, "Cannot set server socket flags");
        close(sock);
        return false;
    }

    if (null == selector) {
        LogError(false, "Selector not created!");
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
        LogError(false, "Cannot register TCP socket on selector");
        close(sock);
        return null;
    }


    int result = connect(sock, address, INET_ADDRSTRLEN);

    if (EINPROGRESS == errno || 0 == result) {
        return CreateTcpConnection(sock, (struct sockaddr_storage *) address, sizeof(*address));
    }

    return null;
}

TcpConnection *ConnectToIPv6TcpServer(struct sockaddr * address, const FdHandler *handler, void *data) {
    LogInfo("Connecting to IPv6 server...");

    if (null == address){
        LogError(false,"Address cannot be null");
        return null;
    }

    if (AF_INET6 != address->sa_family){
        LogError(false,"Only IPv6 addresses are supported");
        return null;
    }


    int sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

    LogInfo("Remote IPv6 connection with file descriptor %d",sock);

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));

    if (-1 == SelectorFdSetNio(sock)) {
        LogError(false, "Cannot set server socket flags");
        close(sock);
        return false;
    }

    LogInfo("Sock with file descriptor %d is non-blocking",sock);

    if (null == selector) {
        LogError(false, "Selector not created!");
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
            LogError(false, "Cannot register TCP socket on selector");
            close(sock);
            return null;
        }
        LogInfo("Remote IPv6 connection succeeded!");
        return CreateTcpConnection(sock, (struct sockaddr_storage *) address, sizeof(*address));
    }

    return null;
}


int ListenOnTcp(unsigned int port, const FdHandler *handler, struct sockaddr *address, socklen_t addressSize,
                 unsigned concurrentConnections) {

    if (port > 65535 || port <= 0) {
        LogError(false, "Invalid port. Cannot be null");
        return -1;
    }

    if (null == address){
        LogError(false,"Address cannot be null");
        return -1;
    }

    if (null == selector) {
        LogError(false, "Selector not created!");
        return -1;
    }

    int servSock = socket(address->sa_family, SOCK_STREAM, IPPROTO_TCP);

    if (servSock < 0) {
        LogError(true, "Cannot open passive socket");
        return -1;
    }

    setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));

    if (AF_INET6 == address->sa_family)
        setsockopt(servSock, SOL_IPV6, IPV6_V6ONLY, &(int) {1}, sizeof(int));

    LogInfo("Opened passive socket %d for TCP server",servSock);

    if (bind(servSock, address, addressSize) < 0) {
        LogError(true, "Cannot bind socket to file descriptor");
        close(servSock);
        return -1;
    }

    LogInfo("Bound TCP server socket to file descriptor");

    if (listen(servSock, concurrentConnections) < 0) {
        LogError(true, "Cannot set passive socket to listen");
        close(servSock);
        return -1;
    }

    LogInfo("Set TCP server socket %d to listen", servSock);

    if (-1 == SelectorFdSetNio(servSock)) {
        LogError(false, "Cannot set server socket flags");
        close(servSock);
        return -1;
    }

    SelectorStatus status = SelectorRegister((fd_selector) selector, servSock, handler, SELECTOR_OP_READ, null);

    if (status != SELECTOR_STATUS_SUCCESS) {
        LogError(false, "Cannot register TCP socket on selector");
        close(servSock);
        return -1;
    }

    return servSock;

}



