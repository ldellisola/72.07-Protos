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


bool ListenOnTcp(unsigned int port, const FdHandler *handler, struct sockaddr *address, socklen_t addressSize);



static fd_selector selector = NULL;
static bool isRunning = true;

const SelectorOptions options = {
        .Signal = SIGALRM,
        .SelectTimeout = {
                .tv_sec = 10,
                .tv_nsec = 0,
        }
};

bool InitTcpServer(const SelectorOptions *optionalOptions) {


    if (0 != SelectorInit(null == optionalOptions ? &options : optionalOptions)) {
        LogError(false, "Cannot initialize Selector");
        return false;
    }

    selector = (struct fdselector *) SelectorNew(1024);
    if (null == selector) {
        LogError(false, "Cannot create selector");
        return false;
    }

    return true;
}


bool IPv4ListenOnTcpPort(unsigned int port, const FdHandler *handler, const char *address) {
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
                return false;
            default:
                LogError(true, "Unknown error parsing IPv4");
                return false;
        }

        addr.sin_addr = in4Addr;
    }

    return ListenOnTcp(port, handler, (struct sockaddr *) &addr, sizeof(addr));
}

bool IPv6ListenOnTcpPort(unsigned int port, const FdHandler *handler, const char *address) {
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
                return false;
            default:
                LogError(true, "Unknown error parsing IPv6");
                return false;
        }
        addr.sin6_addr = in6Addr;
    }

    return ListenOnTcp(port, handler, (struct sockaddr *) &addr, sizeof(addr));

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
        LogError(false, "Cannot set client socket non-blocking in fd %d", fd);
        return null;
    }

    TcpConnection *tcpConnection = CreateTcpConnection(client, &clientAddr, clientAddrLen);

    LogInfo("New TCP connection up and running on file descriptor %d", clientAddr);

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

TcpConnection *ConnectToIPv4TcpServer(byte *address, byte port[2], const FdHandler *handler, void *data) {
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

    struct sockaddr_in addr = {
            .sin_family = AF_INET
    };

    memcpy(&(addr.sin_addr), address, 4);
    memcpy(&(addr.sin_port), port, 2);

    char buff[500];
    memset(buff,0,500);
    inet_ntop(AF_INET,&addr,buff,sizeof(addr));


    LogInfo("Address: %s",buff);

    int result = connect(sock, (struct sockaddr *) &addr, sizeof(addr));

    if (EINPROGRESS == errno || 0 == result) {
        return CreateTcpConnection(sock, (struct sockaddr_storage *) &addr, sizeof(addr));
    }

    return null;
}

TcpConnection *ConnectToIPv6TcpServer(byte *address, byte port[2], const FdHandler *handler, void *data) {
    int sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
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

    char buffer[100];
    memset(buffer,0,100);
    struct in6_addr in6Addr;
    memcpy(&in6Addr,address,16);

    struct sockaddr_in6 addr = {
            .sin6_family = AF_INET6,
    };
    const char * aaa = inet_ntop(AF_INET6,&in6Addr,buffer,INET6_ADDRSTRLEN);

    inet_pton(AF_INET6, aaa, &addr.sin6_addr);

    memcpy(&(addr.sin6_port), port, 2);

//
//    for(int i = 0 ; i <8; i++){
//        byte a = buff[i];
//        buff[i] = buff[15-i];
//        buff[15-i] = a;
//    }

//    LogInfo("Address: %s",aaa);


//    const char * other = "2800:3f0:4001:81f::2003";


    int result = connect(sock, (struct sockaddr *) &addr, sizeof(addr));

    if (EINPROGRESS == errno || 0 == result) {
        return CreateTcpConnection(sock, (struct sockaddr_storage *) &addr, sizeof(addr));
    }

    return null;
}


bool ListenOnTcp(unsigned int port, const FdHandler *handler, struct sockaddr *address, socklen_t addressSize) {

    if (port > 65535 || port <= 0) {
        LogError(false, "Invalid port. Cannot be null");
        return false;
    }

    if (null == address){
        LogError(false,"Address cannot be null");
        return false;
    }

    int servSock = socket(address->sa_family, SOCK_STREAM, IPPROTO_TCP);

    if (servSock < 0) {
        LogError(true, "Cannot open passive socket");
        return false;
    }

    setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));

    if (AF_INET6 == address->sa_family)
        setsockopt(servSock, SOL_IPV6, IPV6_V6ONLY, &(int) {1}, sizeof(int));

    LogInfo("Opened passive socket %d for TCP server",servSock);

    if (bind(servSock, address, addressSize) < 0) {
        LogError(true, "Cannot bind socket to file descriptor");
        close(servSock);
        return false;
    }

    LogInfo("Bound TCP server socket to file descriptor");

    if (listen(servSock, 10) < 0) {
        LogError(true, "Cannot set passive socket to listen");
        close(servSock);
        return false;
    }
    LogInfo("Set TCP server socket to listen");

    if (-1 == SelectorFdSetNio(servSock)) {
        LogError(false, "Cannot set server socket flags");
        close(servSock);
        return false;
    }

    if (null == selector) {
        LogError(false, "Selector not created!");
        close(servSock);
        return false;
    }

    SelectorStatus status = SelectorRegister((fd_selector) selector, servSock, handler, SELECTOR_OP_READ, null);

    if (status != SELECTOR_STATUS_SUCCESS) {
        LogError(false, "Cannot register TCP socket on selector");
        close(servSock);
        return false;
    }

    return true;

}



