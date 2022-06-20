//
// Created by tluci on 11/6/2022.
//

#include "lulu/lulu_server.h"

#include "utils/utils.h"
#include "utils/logger.h"

int ipv4SocketLulu = -1;
int ipv6SocketLulu = -1;
void LuluPassiveAccept(SelectorKey *key);


const FdHandler lulu = {
        .handle_read       = LuluPassiveAccept,
        .handle_write      = NULL,
        .handle_close      = NULL, // nada que liberar
};

bool RegisterLuluServer(const char *port, const char *address, int poolSize,const char *usernames[], const char *passwords[]) {
    CreateLuluConnectionPool(poolSize);
    LoadLuluUsers(usernames, passwords);
    if (null == address){
        bool success = RegisterLuluServerOnIPv4(port,null);
        success &= RegisterLuluServerOnIPv6(port,null);
        return success;
    }

    switch (GetAddressFamily(address)) {
        case AF_INET:
            return RegisterLuluServerOnIPv4(port,address);
        case AF_INET6:
            return RegisterLuluServerOnIPv6(port,address);
        default:
            LogError(false,"Unknown Address type");
            return false;
    }
}

bool RegisterLuluServerOnIPv4(const char * port, const char *address) {
    Debug("Starting LULU server on IPv4...");
    int portNum = atoi(port);
    ipv4SocketLulu = IPv4ListenOnTcpPort(portNum, &lulu, address, 50);

    if ( -1 == ipv4SocketLulu)
        return false;

    Debug("LULU server up and running on IPv4!");
    return true;

}

bool RegisterLuluServerOnIPv6(const char * port, const char *address) {
    Debug("Starting LULU server on IPv6...");
    int portNum = atoi(port);
    ipv6SocketLulu = IPv6ListenOnTcpPort(portNum, &lulu, address, 50);
    if (-1 == ipv6SocketLulu)
        return false;

    Debug("LULU server up and running on IPv6!");
    return true;
}

void LuluPassiveAccept(SelectorKey *key) {

    TcpConnection *client = AcceptNewTcpConnection(key->Fd);

    if (null == client) {
        LogError(false, "Cannot accept client connection");
        return;
    }

    LuluConnection *connection = CreateLuluConnection(client);

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
        DisposeLuluConnection(connection, NULL);
        LogError(false, "Cannot register new LULU connection to the selector");
    }
}

void DisposeLuluServer() {

    if (FUNCTION_ERROR != ipv6SocketLulu)
        close(ipv6SocketLulu);

    if (FUNCTION_ERROR != ipv4SocketLulu)
        close(ipv4SocketLulu);

    DisposeAllLuluUsers();
//    CleanLuluConnectionPool();
}

