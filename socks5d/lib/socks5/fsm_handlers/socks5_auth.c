//
// Created by Lucas Dell'Isola on 13/06/2022.
//

#include <string.h>
#include "socks5/fsm_handlers/socks5_auth.h"
#include "socks5/socks5_connection.h"
#include "utils/logger.h"
#include "socks5/socks5_messages.h"


void AuthReadInit(unsigned int state, void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    AuthData *d = &connection->Data.Auth;

    d->AuthSucceeded = false;
    d->ReadBuffer = &connection->ReadBuffer;
    d->WriteBuffer = &connection->WriteBuffer;
    AuthParserReset(&d->Parser);
}

void AuthReadClose(unsigned int state, void *data) {

}

unsigned AuthReadRun(void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    AuthData *d = &connection->Data.Auth;
    size_t bufferSize;

    byte *buffer = BufferWritePtr(d->ReadBuffer, &bufferSize);
    ssize_t bytesRead = ReadFromTcpConnection(connection->ClientTcpConnection, buffer, bufferSize);

    if (bytesRead <= 0) {
        return CS_ERROR;
    }

    BufferWriteAdv(d->ReadBuffer, bytesRead);
    AuthParserConsume(&d->Parser, buffer, bytesRead);

    BufferReset(d->ReadBuffer);

    if (!AuthParserHasFinished(d->Parser.State))
        return CS_AUTH_READ;

    if (AuthParserHasFailed(d->Parser.State))
        return CS_ERROR;

    if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_WRITE)) {
        connection->User = LogInSocks5User(d->Parser.UName, d->Parser.Passwd);
        d->AuthSucceeded = null != connection->User;

        buffer = BufferWritePtr(d->WriteBuffer, &bufferSize);
        size_t bytesWritten = BuildAuthResponse(buffer, bufferSize, d->AuthSucceeded);

        if (0 == bytesWritten)
            return CS_ERROR;

        BufferWriteAdv(d->WriteBuffer, (ssize_t ) bytesWritten);

        return CS_AUTH_WRITE;
    }

    return CS_ERROR;
}

void AuthWriteClose(unsigned int state, void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    AuthData *d = &connection->Data.Auth;

    BufferReset(d->WriteBuffer);
    BufferReset(d->ReadBuffer);
}

unsigned AuthWriteRun(void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    AuthData *d = &connection->Data.Auth;

    if (!BufferCanRead(d->WriteBuffer)) {
        if (!d->AuthSucceeded) {
            Debug("User not authorized");
            return CS_DONE;
        }
        Debug("User authorized");

        bool success = SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_READ);
        return success ? CS_REQUEST_READ : CS_ERROR;
    }

    size_t size;
    byte *ptr = BufferReadPtr(d->WriteBuffer, &size);
    ssize_t bytesWritten = WriteToTcpConnection(connection->ClientTcpConnection, ptr, size);

    if(FUNCTION_ERROR == bytesWritten)
        return CS_ERROR;

    BufferReadAdv(d->WriteBuffer, bytesWritten);

    return CS_AUTH_WRITE;
}



