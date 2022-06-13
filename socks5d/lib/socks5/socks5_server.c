//
// Created by Lucas Dell'Isola on 30/05/2022.
//

#include <string.h>
#include "socks5/socks5_server.h"
#include "utils/utils.h"
#include "utils/logger.h"

void Socks5PassiveAccept(SelectorKey * key);

const FdHandler socksv5 = {
        .handle_read       = Socks5PassiveAccept,
        .handle_write      = NULL,
        .handle_close      = NULL, // nada que liberar
};

void RegisterSocks5ServerOnIPv4(const char *port) {
    LogInfo("Starting SOCKS5 server...");
    int portNum = atoi(port);

    // TODO: Make address agnostic
    IPv4ListenOnTcpPort(portNum,&socksv5);

    LogInfo("SOCKS5 server up and running!");
}


void Socks5PassiveAccept(SelectorKey *key) {

    TcpConnection * client = AcceptNewTcpConnection(key->Fd);

    if (null == client){
        LogError(false,"Cannot accept client connection");
        return;
    }

    Socks5Connection * connection = Socks5ConnectionInit(client);

    if (null == connection){
        LogError(false,"Cannot create connection");
        return;
    }

    SelectorStatus status = SelectorRegister(
            key->Selector,
            client->FileDescriptor,
            connection->Handler,
            SELECTOR_OP_READ,
            connection
            );

    if (SELECTOR_STATUS_SUCCESS != status){
        Socks5ConnectionDestroy(connection, NULL);
        LogError(false,"Cannot register new SOCKS5 connection to the selector");
    }
}










