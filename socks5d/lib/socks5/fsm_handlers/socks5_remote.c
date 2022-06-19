//
// Created by Lucas Dell'Isola on 14/06/2022.
//

#include "socks5/fsm_handlers/socks5_remote.h"
#include "socks5/socks5_connection.h"
#include "socks5/socks5_metrics.h"

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
    ssize_t bytes = ReadFromTcpConnection(connection->RemoteTcpConnection, buffer, len);

    if (0 == bytes) {
        bool success;
        success = FUNCTION_OK == DisconnectFromTcpConnection(connection->RemoteTcpConnection, SHUT_RD);
        success &= SELECTOR_STATUS_SUCCESS == SelectorSetInterest(selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_NOOP);
        success &= SELECTOR_STATUS_SUCCESS == SelectorSetInterest(selector, connection->ClientTcpConnection->FileDescriptor, SELECTOR_OP_WRITE);
        return success ? CS_CLIENT_WRITE : CS_ERROR;
    }

    if (FUNCTION_ERROR == bytes){
        return CS_ERROR;
    }

    BufferWriteAdv(&connection->WriteBuffer, bytes);
    RegisterBytesTransferredInSocks5Metrics(bytes);

    bool success;
    success = SELECTOR_STATUS_SUCCESS == SelectorSetInterest(selector, connection->ClientTcpConnection->FileDescriptor, SELECTOR_OP_WRITE);
    success &= SELECTOR_STATUS_SUCCESS == SelectorSetInterest(selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_NOOP);

    return success ? CS_CLIENT_WRITE : CS_ERROR;
}

void RemoteReadClose(unsigned int state, void *data) {

}

void RemoteWriteInit(unsigned int state, void *data) {

}

unsigned RemoteWriteRun(void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    fd_selector selector = ((SelectorKey *) data)->Selector;

    if (!BufferCanRead(&connection->WriteBuffer)){
        bool success = true;
        if (connection->ClientTcpConnection->CanRead){
            success &= SELECTOR_STATUS_SUCCESS == SelectorSetInterest(selector, connection->ClientTcpConnection->FileDescriptor, SELECTOR_OP_READ);
            success &= SELECTOR_STATUS_SUCCESS == SelectorSetInterest(selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_READ);
        }
        else {
            success &= FUNCTION_OK == DisconnectFromTcpConnection(connection->RemoteTcpConnection, SHUT_WR);
            success &= SELECTOR_STATUS_SUCCESS == SelectorSetInterest(selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_READ);
            success &= SELECTOR_STATUS_SUCCESS ==  SelectorSetInterest(selector, connection->ClientTcpConnection->FileDescriptor, SELECTOR_OP_NOOP);
        }

        return success ? CS_CONNECTED : CS_ERROR;
    }

    size_t len;
    byte *buffer = BufferReadPtr(&connection->WriteBuffer, &len);

    ssize_t bytes = WriteToTcpConnection(connection->RemoteTcpConnection, buffer, len);

    if (FUNCTION_ERROR == bytes){
        return CS_ERROR;
    }

    BufferReadAdv(&connection->WriteBuffer, bytes);

    return CS_REMOTE_WRITE;
}

void RemoteWriteClose(unsigned int state, void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    BufferReset(&connection->WriteBuffer);
    BufferReset(&connection->ReadBuffer);

}
