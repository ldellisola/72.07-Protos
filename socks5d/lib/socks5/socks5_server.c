//
// Created by Lucas Dell'Isola on 30/05/2022.
//
#include "socks5/socks5_server.h"
#include <string.h>
#include "utils/utils.h"
#include "utils/logger.h"
#include "tcp/tcp.h"
#include "socks5/socks5_connection.h"
#include "socks5/socks5_metrics.h"
#include "socks5/socks5_password_dissector.h"

int ipv4Socket = -1;
int ipv6Socket = -1;



void Socks5PassiveAccept(SelectorKey *key);


const FdHandler socksv5 = {
        .handle_read       = Socks5PassiveAccept,
        .handle_write      = NULL,
        .handle_close      = NULL, // nada que liberar
};

bool RegisterSocks5Server(const char *port, const char *address, int poolSize, time_t timeout,
                          const char *usernames[],
                          const char *passwords[], bool passwordDissectors) {
    CreateSocks5ConnectionPool(poolSize);
    SetConnectionTimeout(timeout);
    LoadSocks5Users(usernames, passwords);
    InitSocks5Metrics();
    EnablePasswordDissector(passwordDissectors);

    if (null == address){
        bool success = RegisterSocks5ServerOnIPv4(port,null);
        success &= RegisterSocks5ServerOnIPv6(port,null);
        return success;
    }

    switch (GetAddressFamily(address)) {
        case AF_INET:
            return RegisterSocks5ServerOnIPv4(port,address);
        case AF_INET6:
            return RegisterSocks5ServerOnIPv6(port,address);
        default:
            LogError(false,"Unknown Address type");
            return false;
    }
}



bool RegisterSocks5ServerOnIPv4(const char *port, const char *address) {
    LogInfo("Starting SOCKS5 server on IPv4...");
    // TODO inspect this
    int portNum = atoi(port);

    ipv4Socket = IPv4ListenOnTcpPort(portNum, &socksv5, address, 50);

    if ( -1 == ipv4Socket)
        return false;

    LogInfo("SOCKS5 server up and running on IPv4!");
    return true;

}

bool RegisterSocks5ServerOnIPv6(const char *port, const char *address) {
    LogInfo("Starting SOCKS5 server on IPv6...");
    int portNum = atoi(port);

    ipv6Socket = IPv6ListenOnTcpPort(portNum, &socksv5, address, 50);
    if (-1 == ipv6Socket)
        return false;

    LogInfo("SOCKS5 server up and running on IPv6!");
    return true;
}

void DisposeSocks5Server() {
    if (-1 != ipv6Socket)
        close(ipv6Socket);

    if (-1 != ipv4Socket)
        close(ipv4Socket);

    DisposeAllSocks5Users();
    CleanSocks5ConnectionPool();
}


void Socks5PassiveAccept(SelectorKey *key) {

    TcpConnection *client = AcceptNewTcpConnection(key->Fd);

    if (null == client) {
        LogError(false, "Cannot accept client connection");
        return;
    }

    Socks5Connection *connection = CreateSocks5Connection(client);

    if (null == connection) {
        LogError(false, "Cannot create connection");
        return;
    }

    SelectorStatus status = SelectorRegister(
            key->Selector,
            client->FileDescriptor,
            connection->Handler,
            SELECTOR_OP_READ,
            connection
    );

    if (SELECTOR_STATUS_SUCCESS != status) {
        DisposeSocks5Connection(connection, NULL);
        LogError(false, "Cannot register new SOCKS5 connection to the selector");
    }
}










