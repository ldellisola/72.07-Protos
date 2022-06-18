//
// Created by Lucas Dell'Isola on 14/06/2022.
//

#include "socks5/fsm_handlers/socks5_connected.h"
#include "socks5/socks5_connection.h"
#include "socks5/socks5_password_dissector.h"

#define ATTACHMENT(key) ( (Socks5Connection*)((SelectorKey*)(key))->Data)

void ConnectedConnectionInit(unsigned int state, void *data) {
//    Socks5Connection *connection = ATTACHMENT(data);
//    if (CanDetectPasswords(connection)) {
//        Pop3AuthParser *d = &connection->Data.Pop3Parser;
//        d->User = null;
//        d->Password = null;
//        ResetPop3AuthParser(d);
//    }
}

void ConnectedConnectionClose(unsigned int state, void *data) {
//    Socks5Connection *connection = ATTACHMENT(data);
//    if (CanDetectPasswords(connection)) {
//        Pop3AuthParser *d = &connection->Data.Pop3Parser;
//        ResetPop3AuthParser(d);
//    }
}


unsigned ConnectedConnectionRun(void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    int requestedFd = ((SelectorKey *) data)->Fd;
    fd_selector selector = ((SelectorKey *) data)->Selector;

    // TODO controlar errores aca
    if (connection->ClientTcpConnection->CanRead && requestedFd == connection->ClientTcpConnection->FileDescriptor) {
        SelectorSetInterest(selector, connection->ClientTcpConnection->FileDescriptor, SELECTOR_OP_READ);
        SelectorSetInterest(selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_NOOP);
        return CS_CLIENT_READ;
    }

    if (connection->RemoteTcpConnection->CanRead && requestedFd == connection->RemoteTcpConnection->FileDescriptor) {
        SelectorSetInterest(selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_READ);
        SelectorSetInterest(selector, connection->ClientTcpConnection->FileDescriptor, SELECTOR_OP_NOOP);
        return CS_REMOTE_READ;
    }


    if (IsTcpConnectionDisconnected(connection->ClientTcpConnection) && IsTcpConnectionDisconnected(connection->RemoteTcpConnection)) {
        return CS_DONE;
    }

    return CS_CONNECTED;
}
