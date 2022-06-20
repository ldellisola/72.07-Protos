//
// Created by Lucas Dell'Isola on 30/05/2022.
//
#include "socks5/socks5_server.h"
#include <errno.h>
#include "utils/utils.h"
#include "utils/logger.h"
#include "tcp/tcp.h"
#include "socks5/socks5_connection.h"
#include "socks5/socks5_metrics.h"
#include "socks5/socks5_password_dissector.h"
#include "socks5/socks5_buffer.h"
#include "socks5/socks5_timeout.h"

static int ipv4Socket = FUNCTION_ERROR;
static int ipv6Socket = FUNCTION_ERROR;



static void Socks5PassiveAccept(SelectorKey *key);


static const FdHandler socksv5 = {
        .handle_read       = Socks5PassiveAccept,
        .handle_write      = NULL,
        .handle_close      = NULL, // nada que liberar
};

bool RegisterSocks5Server(const char *port, const char *address, int poolSize, time_t timeout, const char *usernames[],
                          const char *passwords[], bool passwordDissector, size_t bufferSize) {

    CreateSocks5ConnectionPool(poolSize);
    SetSocks5BufferSize(bufferSize);
    SetSocks5ConnectionTimeout(timeout);
    LoadSocks5Users(usernames, passwords);
    InitSocks5Metrics();
    EnablePasswordDissector(passwordDissector);

    bool success = false;
    if (null == address){
        success= RegisterSocks5ServerOnIPv4(port,null);
        success &= RegisterSocks5ServerOnIPv6(port,null);
        if (success)
            LogInfo("Running Socks5 server on [::]:%s and 0.0.0.0:%s",port,port);
        return success;
    }

    switch (GetAddressFamily(address)) {
        case AF_INET:
             success = RegisterSocks5ServerOnIPv4(port,address);
             if (success)
                 LogInfo("Running Socks5 server on %s:%s",address,port);
            break;
        case AF_INET6:
             success = RegisterSocks5ServerOnIPv6(port,address);
             if (success)
                 LogInfo("Running Socks5 server on [%s]:%s",address,port);
            break;
        default:
            Warning("Unknown Address type");
            success = false;
    }
    return success;
}



bool RegisterSocks5ServerOnIPv4(const char *port, const char *address) {
    Debug("Starting SOCKS5 server on IPv4...");
    int portNum = (int) strtol(port,null,10);

    if (0 == portNum && (EINVAL==errno || ERANGE == errno))
        LogFatalWithReason("Invalid port number: %s",port);


    ipv4Socket = IPv4ListenOnTcpPort(portNum, &socksv5, address, 50);

    if (FUNCTION_ERROR == ipv4Socket)
        return false;

    Debug("SOCKS5 server up and running on IPv4!");
    return true;

}

bool RegisterSocks5ServerOnIPv6(const char *port, const char *address) {
    Debug("Starting SOCKS5 server on IPv6...");
    int portNum = (int) strtol(port,null,10);

    if (0 == portNum && (EINVAL==errno || ERANGE == errno))
        LogFatalWithReason("Invalid port number: %s",port);

    ipv6Socket = IPv6ListenOnTcpPort(portNum, &socksv5, address, 50);
    if (FUNCTION_ERROR == ipv6Socket)
        return false;

    Debug("SOCKS5 server up and running on IPv6!");
    return true;
}

void DisposeSocks5Server() {
    Socks5Metrics  * metrics = GetSocks5Metrics();
    LogInfo("Bytes transferred: %ld",metrics->BytesTransferred);
    LogInfo("Total connections: %ld",metrics->HistoricalConnections);
    LogInfo("Current connections: %ld",metrics->CurrentConnections);


    if (FUNCTION_ERROR != ipv6Socket)
        close(ipv6Socket);

    if (FUNCTION_ERROR != ipv4Socket)
        close(ipv4Socket);

    DisposeAllSocks5Users();
    CleanSocks5ConnectionPool();
}


void Socks5PassiveAccept(SelectorKey *key) {

    TcpConnection *client = AcceptNewTcpConnection(key->Fd);

    if (null == client) {
        Error("Cannot accept client connection");
        return;
    }

    Socks5Connection *connection = CreateSocks5Connection(client);

    if (null == connection) {
        Error("Cannot create connection");
        return;
    }

    SelectorStatus status = SelectorRegister(
            key->Selector,
            client->FileDescriptor,
            connection->Handler,
            SELECTOR_OP_READ,
            connection,
            true
    );

    if (SELECTOR_STATUS_SUCCESS != status) {
        DisposeSocks5Connection(connection, NULL);
        Error("Cannot register new SOCKS5 connection to the selector");
    }
}










