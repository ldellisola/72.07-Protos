//
// Created by Lucas Dell'Isola on 14/06/2022.
//

#include "socks5/fsm_handlers/socks5_remote.h"
#include "socks5/socks5_connection.h"

#define ATTACHMENT(key) ( (Socks5Connection*)((SelectorKey*)(key))->Data)

void RemoteReadInit(unsigned int state, void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    BufferReset(&connection->WriteBuffer);
    BufferReset(&connection->ReadBuffer);
}

unsigned RemoteReadRun(void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    fd_selector selector = ((SelectorKey *) data)->Selector;

    size_t len;
    byte *buffer = BufferWritePtr(&connection->WriteBuffer, &len);
    size_t bytes = ReadFromTcpConnection(connection->RemoteTcpConnection, buffer, len);

    if (0 == bytes) {
        // TODO Handle error
        DisconnectFromTcpConnection(connection->RemoteTcpConnection, SHUT_RD);
        SelectorSetInterest(selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_NOOP);
        SelectorSetInterest(selector, connection->ClientTcpConnection->FileDescriptor, SELECTOR_OP_WRITE);
        return CS_CLIENT_WRITE;
    }

    BufferWriteAdv(&connection->WriteBuffer, bytes);

    // TODO Handle error
    SelectorSetInterest(selector, connection->ClientTcpConnection->FileDescriptor, SELECTOR_OP_WRITE);
    SelectorSetInterest(selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_NOOP);

    return CS_CLIENT_WRITE;
}

void RemoteReadClose(unsigned int state, void *data) {

}

void RemoteWriteInit(unsigned int state, void *data) {

}

unsigned RemoteWriteRun(void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    fd_selector selector = ((SelectorKey *) data)->Selector;

    if (!BufferCanRead(&connection->WriteBuffer)){
        if (connection->ClientTcpConnection->CanRead){
            // TODO handle error
            SelectorSetInterest(selector, connection->ClientTcpConnection->FileDescriptor, SELECTOR_OP_READ);
            SelectorSetInterest(selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_READ);
        }
        else {
            DisconnectFromTcpConnection(connection->RemoteTcpConnection, SHUT_WR);
            SelectorSetInterest(selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_READ);
            SelectorSetInterest(selector, connection->ClientTcpConnection->FileDescriptor, SELECTOR_OP_NOOP);
        }

        return CS_CONNECTED;
    }

    size_t len;
    byte *buffer = BufferReadPtr(&connection->WriteBuffer, &len);

    size_t bytes = WriteToTcpConnection(connection->RemoteTcpConnection, buffer, len);


    BufferReadAdv(&connection->WriteBuffer, bytes);

    return CS_REMOTE_WRITE;
}

void RemoteWriteClose(unsigned int state, void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    BufferReset(&connection->WriteBuffer);
    BufferReset(&connection->ReadBuffer);

}
