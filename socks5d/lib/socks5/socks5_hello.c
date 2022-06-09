//
// Created by Lucas Dell'Isola on 09/06/2022.
//

#include <strings.h>
#include "selector/selector.h"
#include "socks5/socks5_connection.h"
#include "utils/logger.h"
#include "socks5/socks5_connection_status.h"
#include "socks5/socks5_messages.h"

#include "socks5/socks5_hello.h"

typedef enum {
    SOCKS5_AUTH_NO_AUTH = 0,
    SOCKS5_AUTH_USER_PASS = 0x02,
    SOCKS5_AUTH_INVALID = 0xFF
}SOCKS5_AUTH;

#define ATTACHMENT(key) ( (Socks5Connection*)((SelectorKey*)(key))->Data)
SOCKS5_AUTH SelectAuthenticationMethod(byte * methods, int methodLength);



void HelloReadInit(unsigned int state, void *data) {
    Socks5Connection * connection = ATTACHMENT(data);
    HelloData* d = &connection->Data.Hello;

    d->ReadBuffer = &connection->ReadBuffer;
    d->WriteBuffer = &connection->WriteBuffer;
    d->Parser = HelloParserInit();
}

void HelloReadClose(unsigned int state, void *data) {
    Socks5Connection * connection = ATTACHMENT(data);
    HelloData* d = &connection->Data.Hello;
    HelloParserDestroy(d->Parser);
    bzero(d, sizeof(HelloData));
}

unsigned HelloReadRun(void *data) {
    Socks5Connection * connection = ATTACHMENT(data);
    HelloData* d = &connection->Data.Hello;
    size_t bufferSize;

    byte * buffer = BufferWritePtr(d->ReadBuffer, &bufferSize);
    ssize_t bytesRead = ReadFromTcpConnection(connection->TcpConnection, buffer, bufferSize);

    if (bytesRead < 0)
    {
        LogError(false,"Cannot read from Tcp connection");
        return CS_ERROR;
    }

    BufferWriteAdv(d->ReadBuffer,bytesRead);
    HelloParserConsume(d->Parser,buffer,bytesRead);

    if (!HelloParserHasFinished(d->Parser->State))
        return CS_HELLO_READ;

    if (HelloParserHasFailed(d->Parser->State))
        return CS_ERROR;

    if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data,SELECTOR_OP_WRITE)){
        int authMethod = SelectAuthenticationMethod(d->Parser->Methods, d->Parser->NMethods);

        buffer = BufferWritePtr(d->WriteBuffer,&bufferSize);
        int bytesWritten = BuildHelloResponse(buffer,bufferSize,authMethod);
        if (bytesWritten < 0)
            return CS_ERROR;
        BufferWriteAdv(d->WriteBuffer,bytesWritten);
         // TODO: Ver donde va esto?
//        WriteToTcpConnection(connection->TcpConnection, buffer, bytesWritten);

        return CS_HELLO_WRITE;
    }

    return CS_ERROR;
}

SOCKS5_AUTH SelectAuthenticationMethod(byte *methods, int methodLength) {
    for(int i = 0; i < methodLength; i++)
        if ( SOCKS5_AUTH_USER_PASS == methods[i] || SOCKS5_AUTH_NO_AUTH == methods[i])
            return methods[i];

    return SOCKS5_AUTH_INVALID;
}

