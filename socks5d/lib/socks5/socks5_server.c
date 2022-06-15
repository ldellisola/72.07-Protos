//
// Created by Lucas Dell'Isola on 30/05/2022.
//

#include <string.h>
#include "utils/utils.h"
#include "utils/logger.h"
#include "tcp/tcp.h"
#include "socks5/socks5_connection.h"
#include "socks5/socks5_server.h"

void Socks5PassiveAccept(SelectorKey *key);

const FdHandler socksv5 = {
        .handle_read       = Socks5PassiveAccept,
        .handle_write      = NULL,
        .handle_close      = NULL, // nada que liberar
};

bool RegisterSocks5Server(const char * port, const char * address){
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
    //TODO inspect this
    int portNum = atoi(port);

    if (IPv4ListenOnTcpPort(portNum, &socksv5, address) == false)
        return false;

    LogInfo("SOCKS5 server up and running on IPv4!");
    return true;

}

bool RegisterSocks5ServerOnIPv6(const char *port, const char *address) {
    LogInfo("Starting SOCKS5 server on IPv6...");
    int portNum = atoi(port);

    if (IPv6ListenOnTcpPort(portNum, &socksv5, address) == false)
        return false;

    LogInfo("SOCKS5 server up and running on IPv6!");
    return true;
}


void Socks5PassiveAccept(SelectorKey *key) {

    TcpConnection *client = AcceptNewTcpConnection(key->Fd);

    if (null == client) {
        LogError(false, "Cannot accept client connection");
        return;
    }

    Socks5Connection *connection = Socks5ConnectionInit(client);

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
        Socks5ConnectionDestroy(connection, NULL);
        LogError(false, "Cannot register new SOCKS5 connection to the selector");
    }
}










