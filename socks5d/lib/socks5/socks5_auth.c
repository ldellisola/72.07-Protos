//
// Created by Lucas Dell'Isola on 13/06/2022.
//

#include <string.h>
#include "socks5/socks5_auth.h"
#include "socks5/socks5_connection.h"
#include "utils/logger.h"
#include "socks5/socks5_messages.h"

#define ATTACHMENT(key) ( (Socks5Connection*)((SelectorKey*)(key))->Data)

bool LogInUser(char name[256], char passwd[256]);

void AuthReadInit(unsigned int state, void *data) {
    Socks5Connection * connection = ATTACHMENT(data);
    AuthData * d = &connection->Data.Auth;

    d->AuthSucceeded = false;
    d->ReadBuffer = &connection->ReadBuffer;
    d->WriteBuffer = &connection->WriteBuffer;
    AuthParserReset(&d->Parser);
}

void AuthReadClose(unsigned int state, void *data) {

}

unsigned AuthReadRun(void *data) {
    Socks5Connection * connection = ATTACHMENT(data);
    AuthData* d = &connection->Data.Auth;
    size_t bufferSize;

    byte * buffer = BufferWritePtr(d->ReadBuffer, &bufferSize);
    ssize_t bytesRead = ReadFromTcpConnection(connection->ClientTcpConnection, buffer, bufferSize);

    if (bytesRead < 0)
    {
        LogError(false,"Cannot read from Tcp connection");
        return CS_ERROR;
    }

    BufferWriteAdv(d->ReadBuffer,bytesRead);
    AuthParserConsume(&d->Parser,buffer,bytesRead);

    if (!AuthParserHasFinished(d->Parser.State))
        return CS_AUTH_READ;

    if (AuthParserHasFailed(d->Parser.State))
        return CS_ERROR;

    if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data,SELECTOR_OP_WRITE)){

        d->AuthSucceeded = LogInUser(d->Parser.UName, d->Parser.Passwd);

        buffer = BufferWritePtr(d->WriteBuffer,&bufferSize);
        size_t bytesWritten = BuildAuthResponse(buffer,bufferSize,d->AuthSucceeded);

        if (0 == bytesWritten)
            return CS_ERROR;
        BufferWriteAdv(d->WriteBuffer,bytesWritten);

        return CS_AUTH_WRITE;
    }

    return CS_ERROR;
}

void AuthWriteClose(unsigned int state, void *data) {
    Socks5Connection * connection = ATTACHMENT(data);
    AuthData * d = &connection->Data.Auth;

    BufferReset(d->WriteBuffer);
    BufferReset(d->ReadBuffer);
}

unsigned AuthWriteRun(void *data) {
    Socks5Connection * connection = ATTACHMENT(data);
    HelloData* d = &connection->Data.Hello;

    if (!BufferCanRead(d->WriteBuffer))
    {
        if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data,SELECTOR_OP_READ)) {
            return CS_REQUEST_READ;
        }
        return CS_ERROR;
    }

    size_t size;
    byte * ptr = BufferReadPtr(d->WriteBuffer,&size);

    size_t bytesWritten = WriteToTcpConnection(connection->ClientTcpConnection, ptr, size);
    BufferReadAdv(d->WriteBuffer,bytesWritten);

    return CS_AUTH_WRITE;
}

#define SOCKS5_DEFAULT_USER "admin"
#define SOCKS5_DEFAULT_PASSWORD "admin"

bool LogInUser(char name[256], char passwd[256]) {

    return 0 == strncmp(name,SOCKS5_DEFAULT_USER,5) && 0 == strncmp(passwd,SOCKS5_DEFAULT_PASSWORD,5);
}
