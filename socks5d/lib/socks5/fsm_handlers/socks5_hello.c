//
// Created by Lucas Dell'Isola on 09/06/2022.
//

#include <strings.h>
#include "selector/selector.h"
#include "socks5/socks5_connection.h"
#include "utils/logger.h"
#include "socks5/socks5_connection_status.h"
#include "socks5/socks5_messages.h"

#include "socks5/fsm_handlers/socks5_hello.h"

typedef enum {
    SOCKS5_AUTH_NO_AUTH = 0,
    SOCKS5_AUTH_USER_PASS = 0x02,
    SOCKS5_AUTH_INVALID = 0xFF
} SOCKS5_AUTH;

SOCKS5_AUTH SelectAuthenticationMethod(byte *methods, int methodLength);



void HelloReadInit(unsigned int state, void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    HelloData *d = &connection->Data.Hello;

    d->ReadBuffer = &connection->ReadBuffer;
    d->WriteBuffer = &connection->WriteBuffer;
    HelloParserReset(&d->Parser);
}

void HelloReadClose(unsigned int state, void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    HelloData *d = &connection->Data.Hello;
    HelloParserReset(&d->Parser);
}

unsigned HelloReadRun(void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    HelloData *d = &connection->Data.Hello;
    size_t bufferSize;

    byte *buffer = BufferWritePtr(d->ReadBuffer, &bufferSize);
    ssize_t bytesRead = ReadFromTcpConnection(connection->ClientTcpConnection, buffer, bufferSize);

    if (FUNCTION_ERROR == bytesRead) {
        return CS_ERROR;
    }

    BufferWriteAdv(d->ReadBuffer, bytesRead);
    HelloParserConsume(&d->Parser, buffer, bytesRead);

    BufferReset(d->ReadBuffer);

    if (!HelloParserHasFinished(d->Parser.State))
        return CS_HELLO_READ;

    if (HelloParserHasFailed(d->Parser.State))
        return CS_ERROR;

    if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_WRITE)) {
        d->AuthenticationMethod = SelectAuthenticationMethod(d->Parser.Methods, d->Parser.NMethods);

        buffer = BufferWritePtr(d->WriteBuffer, &bufferSize);
        size_t bytesWritten = BuildHelloResponse(buffer, bufferSize, d->AuthenticationMethod);
        if (0 == bytesWritten)
            return CS_ERROR;
        BufferWriteAdv(d->WriteBuffer, (ssize_t) bytesWritten);

        return CS_HELLO_WRITE;
    }

    return CS_ERROR;
}

SOCKS5_AUTH SelectAuthenticationMethod(byte *methods, int methodLength) {
    for (int i = methodLength-1; i >=0 ; i--)
        if (SOCKS5_AUTH_USER_PASS == methods[i] || SOCKS5_AUTH_NO_AUTH == methods[i])
            return methods[i];

    return SOCKS5_AUTH_INVALID;
}


unsigned HelloWriteRun(void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    HelloData *d = &connection->Data.Hello;

    if (!BufferCanRead(d->WriteBuffer)) {
        if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_READ)) {
            return d->AuthenticationMethod == SOCKS5_AUTH_USER_PASS ? CS_AUTH_READ : CS_REQUEST_READ;
        }
        return CS_ERROR;
    }

    size_t size;
    byte *ptr = BufferReadPtr(d->WriteBuffer, &size);
    ssize_t bytesWritten = WriteToTcpConnection(connection->ClientTcpConnection, ptr, size);

    if (FUNCTION_ERROR == bytesWritten){
        return CS_ERROR;
    }

    BufferReadAdv(d->WriteBuffer, bytesWritten);

    return CS_HELLO_WRITE;

}

void HelloWriteClose(unsigned int state, void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    HelloData *d = &connection->Data.Hello;

    BufferReset(d->WriteBuffer);
    BufferReset(d->ReadBuffer);
}




