//
// Created by Lucas Dell'Isola on 28/05/2022.
//

#include <memory.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include "utils/utils.h"
#include "utils/logger.h"
#include "selector/selector.h"

#include "tcp/tcp.h"


fd_selector selector      = NULL;
bool isRunning = true;

void InitTcpServer(const SelectorOptions * optionalOptions) {
    const SelectorOptions options = {
            .Signal = SIGALRM,
            .SelectTimeout = {
                    .tv_sec = 10,
                    .tv_nsec = 0,
            }
    };

    if (0 != SelectorInit(null == optionalOptions ? &options : optionalOptions)){
        LogError(false,"Cannot initialize Selector");
        return;
    }

    selector = SelectorNew(1024);
    if (null == selector){
        LogError(false,"Cannot create selector");
    }

}


bool IPv4ListenOnTcpPort(unsigned int port, const FdHandler *handler){
    LogInfo("Staring TCP server...");
    // TODO: See max value
    if ( port > 65000) {
        LogError(false,"Invalid port. Cannot be null");
        return false;
    }

    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);


    // TODO: allow multiple
    int servSock = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);

    if (servSock < 0) {
        LogError(true,"Cannot open passive socket");
        return false;
    }
    setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

    LogInfo("Opened passive socket for TCP server");

    if (bind(servSock, (struct sockaddr *) &addr, sizeof(addr)) < 0){
        LogError(true,"Cannot bind socket to file descriptor");
        close(servSock);
        return false;
    }

    LogInfo("Bound TCP server socket to file descriptor");

    if (listen(servSock, 10) < 0) {
        LogError(true,"Cannot set passive socket to listen");
        close(servSock);
        return false;
    }
    LogInfo("Set TCP server socket to listen");

    if (-1 == SelectorFdSetNio(servSock)) {
        LogError(false, "Cannot get server socket flags");
        close(servSock);
        return false;
    }

    if (null == selector)
    {
        LogError(false,"Selector not created!");
        close(servSock);
        return false;
    }

    SelectorStatus status = SelectorRegister(selector,servSock,handler,SELECTOR_OP_READ,null);

    if (status != SELECTOR_STATUS_SUCCESS){
        LogError(false,"Cannot register TCP socket on selector");
        close(servSock);
        return false;
    }

    return true;
}

bool RunTcpServer() {
    SelectorStatus selectorStatus;
    while (isRunning){
        selectorStatus = SelectorSelect(selector);
        if (selectorStatus != SELECTOR_STATUS_SUCCESS){
            LogError(false,"Error on selector. Exiting...");
            return false;
        }
    }
    return true;
}

void StopTcpServer() {
    isRunning = false;
}


TcpConnection *  AcceptNewTcpConnection(int fd){
    LogInfo("Waiting for new TCP connections...");
    struct sockaddr_storage clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    const int client = accept(fd,(struct sockaddr * )&clientAddr,&clientAddrLen);

    if (-1 == client){
        LogError(false,"Cannot accept client connection in fd %d",fd);
        return null;
    }

    if (-1 == SelectorFdSetNio(client)){
        LogError(false,"Cannot set client socket non-blocking in fd %d",fd);
        return null;
    }

    TcpConnection * tcpConnection = CreateTcpConnection(client, &clientAddr, clientAddrLen);



    LogInfo("New TCP connection up and running on file descriptor %d",clientAddr);

    return tcpConnection;
}



ssize_t ReadFromTcpConnection(TcpConnection * socket, byte * buffer, size_t bufferLength){
    if(socket == null) {
        LogError(false,"Cannot ReadHead from null TCP socket");
        return ERROR;
    }

    if (buffer == null)
    {
        LogError(false,"TCP ReadHead buffer cannot be null");
        return ERROR;
    }

    LogInfo("Reading from TCP socket on file descriptor %d...",socket->FileDescriptor);
    long bytes =  recv(socket->FileDescriptor,buffer,bufferLength,0);

    if (bytes < 0){
        LogError(true,"Could not ReadHead from TCP socket on file descriptor %d",socket->FileDescriptor);
        return ERROR;
    }

    LogInfo("Read %d bytes from TCP socket on file descriptor %d",bytes,socket->FileDescriptor);
    return bytes;
}

size_t WriteToTcpConnection(TcpConnection * socket, byte * content, size_t contentLength){
    if(socket == null) {
        LogError(false,"Cannot WriteHead to null TCP socket");
        return ERROR;
    }

    if (content == null)
    {
        LogError(false,"TCP WriteHead buffer cannot be null");
        return ERROR;
    }

    LogInfo("Writing %d bytes to TCP socket on file descriptor %d...",contentLength, socket->FileDescriptor);

    long bytes = send(socket->FileDescriptor,content,contentLength,0);

    if(bytes < 0){
        LogError(true,"Could not WriteHead to TCP socket on file descriptor %d",socket->FileDescriptor);
        return ERROR;
    }

    LogInfo("Wrote %d bytes to TCP socket on file descriptor %d",bytes,socket->FileDescriptor);

    return bytes;
}




