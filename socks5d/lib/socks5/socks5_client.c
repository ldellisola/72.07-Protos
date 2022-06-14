//
// Created by Lucas Dell'Isola on 14/06/2022.
//

#include "socks5/socks5_client.h"
#include "socks5/socks5_connection.h"

#define ATTACHMENT(key) ( (Socks5Connection*)((SelectorKey*)(key))->Data)

void ClientReadInit(unsigned int state, void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    BufferReset(&connection->WriteBuffer);
    BufferReset(&connection->ReadBuffer);
}

unsigned ClientReadRun(void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    fd_selector selector = ((SelectorKey *) data)->Selector;

    size_t len;
    byte *buffer = BufferWritePtr(&connection->WriteBuffer, &len);
    size_t bytes = ReadFromTcpConnection(connection->ClientTcpConnection, buffer, len);

    if (0 == bytes) {
        // TODO Handle error
        DisconnectFromTcpConnection(connection->ClientTcpConnection, SHUT_RD);
        SelectorSetInterest(selector, connection->ClientTcpConnection->FileDescriptor, SELECTOR_OP_NOOP);
        SelectorSetInterest(selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_WRITE);
        return CS_REMOTE_WRITE;
    }


    BufferWriteAdv(&connection->WriteBuffer, bytes);

    // TODO Handle error
    SelectorSetInterest(selector, connection->ClientTcpConnection->FileDescriptor, SELECTOR_OP_NOOP);
    SelectorSetInterest(selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_WRITE);

    return CS_REMOTE_WRITE;
}

void ClientReadClose(unsigned int state, void *data) {

}

void ClientWriteInit(unsigned int state, void *data) {
}

unsigned ClientWriteRun(void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    fd_selector selector = ((SelectorKey *) data)->Selector;

    size_t len;
    byte *buffer = BufferReadPtr(&connection->WriteBuffer, &len);
    size_t bytes = WriteToTcpConnection(connection->ClientTcpConnection, buffer, len);

    if (0 == bytes) {
        // TODO handle error
        DisconnectFromTcpConnection(connection->ClientTcpConnection, SHUT_WR);
        SelectorSetInterest(selector, connection->ClientTcpConnection->FileDescriptor, SELECTOR_OP_NOOP);
        SelectorSetInterest(selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_READ);
        return CS_CONNECTED;
    }

    BufferReadAdv(&connection->WriteBuffer, bytes);

    // TODO handle error
    SelectorSetInterest(selector, connection->ClientTcpConnection->FileDescriptor, SELECTOR_OP_READ);
    SelectorSetInterest(selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_READ);

    return CS_CONNECTED;

}

void ClientWriteClose(unsigned int state, void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    BufferReset(&connection->WriteBuffer);
    BufferReset(&connection->ReadBuffer);
}
