//
// Created by Lucas Dell'Isola on 14/06/2022.
//

#include "socks5/socks5_request.h"
#include "socks5/socks5_connection.h"
#include "utils/logger.h"
#include "socks5/socks5_messages.h"

unsigned WriteMessageToBuffer(void *selectorKey, RequestData *requestData, size_t bufferSize, byte *buffer, int cmd);

#define ATTACHMENT(key) ( (Socks5Connection*)((SelectorKey*)(key))->Data)

void RequestReadInit(unsigned int state, void *data) {

    Socks5Connection *connection = ATTACHMENT(data);
    RequestData *d = &connection->Data.Request;
    d->Command = 0;
    d->ReadBuffer = &connection->ReadBuffer;
    d->WriteBuffer = &connection->WriteBuffer;
    RequestParserReset(&d->Parser);
}

void RequestReadClose(unsigned int state, void *data) {

}


unsigned RequestReadRun(void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    RequestData *d = &connection->Data.Request;
    size_t bufferSize;
    byte *buffer = BufferWritePtr(d->ReadBuffer, &bufferSize);
    ssize_t bytesRead = ReadFromTcpConnection(connection->ClientTcpConnection, buffer, bufferSize);

    if (bytesRead < 0) {
        LogError(false, "Cannot read from Tcp connection");
        return CS_ERROR;
    }

    BufferWriteAdv(d->ReadBuffer, bytesRead);
    RequestParserConsume(&d->Parser, buffer, bytesRead);

    if (!RequestParserHasFinished(d->Parser.State))
        return CS_REQUEST_READ;

    if (RequestParserFailed(d->Parser.State))
        return CS_ERROR;

    if (d->Parser.CMD != SOCKS5_CMD_CONNECT) {
        d->Command = SOCKS5_REPLY_COMMAND_NOT_SUPPORTED;
        return SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_WRITE) ? CS_REQUEST_WRITE : CS_ERROR;
    }

    if (d->Parser.AType != SOCKS5_ADDRESS_TYPE_IPV4) {
        d->Command = SOCKS5_REPLY_ADDRESS_TYPE_NOT_SUPPORTED;
        return SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_WRITE) ? CS_REQUEST_WRITE : CS_ERROR;
    }

    TcpConnection *remoteConnection = ConnectToIPv4TcpServer(
            d->Parser.DestAddress,
            d->Parser.DestPort,
            connection->Handler,
            connection
    );

    if (null == remoteConnection) {
        LogError(false, "Cannot connect to remote server");
        d->Command = SOCKS5_REPLY_GENERAL_FAILURE;
        return SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_WRITE) ? CS_REQUEST_WRITE : CS_ERROR;
    }

    connection->RemoteTcpConnection = remoteConnection;

    return SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_NOOP) ? CS_ESTABLISH_CONNECTION
                                                                                     : CS_ERROR;
}

void RequestWriteClose(unsigned int state, void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    RequestData *d = &connection->Data.Request;

    BufferReset(d->WriteBuffer);
    BufferReset(d->ReadBuffer);
}

void RequestWriteInit(unsigned int state, void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    RequestData *d = &connection->Data.Request;

    d->WriteBuffer = &connection->WriteBuffer;

    size_t size;
    byte *buffer = BufferWritePtr(d->WriteBuffer, &size);
    size_t messageSize = BuildRequestResponseFromParser(buffer, size, d->Command, &d->Parser);
    BufferWriteAdv(d->WriteBuffer, messageSize);

}

unsigned RequestWriteRun(void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    RequestData *d = &connection->Data.Request;
    fd_selector selector = ((SelectorKey *) data)->Selector;

    if (!BufferCanRead(d->WriteBuffer)) {
        // TODO ver esto
        if (
                SELECTOR_STATUS_SUCCESS ==
                SelectorSetInterest(selector, connection->ClientTcpConnection->FileDescriptor, SELECTOR_OP_READ) &&
                SELECTOR_STATUS_SUCCESS ==
                SelectorSetInterest(selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_READ)
                ) {
            return CS_CONNECTED;
        }
        return CS_ERROR;
    }

    size_t size;
    byte *ptr = BufferReadPtr(d->WriteBuffer, &size);

    size_t bytesWritten = WriteToTcpConnection(connection->ClientTcpConnection, ptr, size);
    BufferReadAdv(d->WriteBuffer, bytesWritten);

    return CS_REQUEST_WRITE;
}


