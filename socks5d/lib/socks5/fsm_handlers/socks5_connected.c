//
// Created by Lucas Dell'Isola on 14/06/2022.
//

#include "socks5/fsm_handlers/socks5_connected.h"
#include "socks5/socks5_connection.h"
#include "socks5/socks5_password_dissector.h"

#define ATTACHMENT(key) ( (Socks5Connection*)((SelectorKey*)(key))->Data)

void ConnectedConnectionInit(unsigned int state, void *data) {
//    Socks5Connection *connection = ATTACHMENT(data);

}

void ConnectedConnectionClose(unsigned int state, void *data) {
//    Socks5Connection *connection = ATTACHMENT(data);

}


unsigned ConnectedConnectionRun(void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    int requestedFd = ((SelectorKey *) data)->Fd;
    fd_selector selector = ((SelectorKey *) data)->Selector;


    if (connection->ClientTcpConnection->CanRead && requestedFd == connection->ClientTcpConnection->FileDescriptor) {
        bool success = true;
        success &= SELECTOR_STATUS_SUCCESS == SelectorSetInterest(selector, connection->ClientTcpConnection->FileDescriptor, SELECTOR_OP_READ);
        success &= SELECTOR_STATUS_SUCCESS == SelectorSetInterest(selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_NOOP);
        return success ? CS_CLIENT_READ : CS_ERROR;
    }

    if (connection->RemoteTcpConnection->CanRead && requestedFd == connection->RemoteTcpConnection->FileDescriptor) {
        bool success = true;
        success &= SELECTOR_STATUS_SUCCESS == SelectorSetInterest(selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_READ);
        success &= SELECTOR_STATUS_SUCCESS == SelectorSetInterest(selector, connection->ClientTcpConnection->FileDescriptor, SELECTOR_OP_NOOP);
        return success ? CS_REMOTE_READ : CS_ERROR;
    }

    if (IsTcpConnectionDisconnected(connection->ClientTcpConnection) && IsTcpConnectionDisconnected(connection->RemoteTcpConnection))
        return CS_DONE;

    return CS_CONNECTED;
}
