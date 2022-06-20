//
// Created by tluci on 19/6/2022.
//

#include "lulu/fsm_handlers/lulu_hello.h"
#include "lulu/lulu_connection.h"
#include "utils/logger.h"
#include "lulu/lulu_messages.h"

int RunParser(ClientHelloData *d, byte *buffer, ssize_t bytesRead, LuluConnection *connection, void *data, size_t bufferSize );

#define ATTACHMENT_LULU_HELLO(key) ( (LuluConnection*)((SelectorKey*)(key))->Data)

void LuluHelloReadInit(unsigned int state, void *data) {
    LuluConnection *connection = ATTACHMENT_LULU_HELLO(data);
    ClientHelloData *d = &connection->Data.Auth;
    d->ParserIndex = 0;
    d->ClientHelloSucceeded = false;
    d->ReadBuffer = &connection->ReadBuffer;
    d->WriteBuffer = &connection->WriteBuffer;
    ClientHelloParserReset(&d->HelloParser);
    ClientGoodbyeParserReset(&d->GoodbyeParser);
}

unsigned LuluHelloReadRun(void *data) {
    LuluConnection *connection = ATTACHMENT_LULU_HELLO(data);
    ClientHelloData *d = &connection->Data.Auth;
    size_t bufferSize;

    byte *buffer = BufferWritePtr(d->ReadBuffer, &bufferSize);
    ssize_t bytesRead = ReadFromTcpConnection(connection->ClientTcpConnection, buffer, bufferSize);

    if (bytesRead <= 0) {
        Error("Cannot read from Tcp connection");
        return LULU_CS_ERROR;
    }

    BufferWriteAdv(d->ReadBuffer, bytesRead);

    int possibleReturn = NO_RETURN;

    while (d->ParserIndex <= PARSER_COUNT ){
         possibleReturn = RunParser(d, buffer, bytesRead, connection, data, bufferSize);
         if(possibleReturn != NO_RETURN){
             return possibleReturn;
         }
    }

    // no reconocio el comando
    buffer = BufferWritePtr(d->WriteBuffer, &bufferSize);
    size_t bytesWritten = BuildClientNotRecognisedResponse(buffer, bufferSize);

    if (0 == bytesWritten)
        return LULU_CS_ERROR;

    BufferWriteAdv(d->WriteBuffer, (ssize_t ) bytesWritten);
    return LULU_CS_HELLO_WRITE;
}

int RunParser(ClientHelloData *d, byte *buffer, ssize_t bytesRead, LuluConnection *connection, void *data, size_t bufferSize ){
    switch (d->ParserIndex) {
        case HELLO_PARSER:
            ClientHelloParserConsume(&d->HelloParser, buffer, bytesRead);

            if (!ClientHelloParserHasFinished(d->HelloParser.State))
                return LULU_CS_HELLO_READ;

            if (ClientHelloParserHasFailed(d->HelloParser.State)){
                d->ParserIndex++;
                return NO_RETURN;
            }
            BufferReset(d->ReadBuffer);
            if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_WRITE)) {
                connection->User = LogInLuluUser(d->HelloParser.UName, d->HelloParser.Passwd);
                d->ClientHelloSucceeded = null != connection->User;

                buffer = BufferWritePtr(d->WriteBuffer, &bufferSize);
                size_t bytesWritten = BuildClientHelloResponse(buffer, bufferSize, d->ClientHelloSucceeded);

                if (0 == bytesWritten)
                    return LULU_CS_ERROR;

                BufferWriteAdv(d->WriteBuffer, (ssize_t ) bytesWritten);
                return LULU_CS_HELLO_WRITE;
            }
            break;
        case GOODBYE_PARSER:
            ClientGoodbyeParserConsume(&d->GoodbyeParser, buffer, bytesRead);

            if (!ClientGoodbyeParserHasFinished(d->GoodbyeParser.State))
                return LULU_CS_HELLO_READ;

            if (ClientGoodbyeParserHasFailed(d->GoodbyeParser.State)){
                d->ParserIndex++;
                return NO_RETURN;
            }
            BufferReset(d->ReadBuffer);
            if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_WRITE)) {
                buffer = BufferWritePtr(d->WriteBuffer, &bufferSize);
                size_t bytesWritten = BuildClientGoodbyeResponse(buffer, bufferSize);

                if (0 == bytesWritten)
                    return LULU_CS_ERROR;

                BufferWriteAdv(d->WriteBuffer, (ssize_t ) bytesWritten);

                return LULU_CS_HELLO_WRITE;
            }
            break;
        default:
            BufferReset(d->ReadBuffer);
            if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_WRITE)) {
                buffer = BufferWritePtr(d->WriteBuffer, &bufferSize);
                size_t bytesWritten = BuildClientNotRecognisedResponse(buffer, bufferSize);

                if (0 == bytesWritten)
                    return LULU_CS_ERROR;

                BufferWriteAdv(d->WriteBuffer, (ssize_t ) bytesWritten);

                return LULU_CS_HELLO_WRITE;
            }
    }
    return LULU_CS_ERROR;
}

void LuluHelloReadClose(unsigned int state, void *data) {

}

void LuluHelloWriteClose(unsigned int state, void *data) {
    LuluConnection *connection = ATTACHMENT_LULU_HELLO(data);
    ClientHelloData *d = &connection->Data.Auth;
    d->ParserIndex = 0;
    BufferReset(d->WriteBuffer);
    ClientHelloParserReset(&d->HelloParser);
    ClientGoodbyeParserReset(&d->GoodbyeParser);
}

unsigned LuluHelloWriteRun(void *data) {
    LuluConnection *connection = ATTACHMENT_LULU_HELLO(data);
    ClientHelloData *d = &connection->Data.Auth;

    if (!BufferCanRead(d->WriteBuffer)) {
        if (!d->ClientHelloSucceeded && d->ParserIndex == HELLO_PARSER) {
            Debug("User not authorized");
            return LULU_CS_HELLO_READ;
        }
        if(d->ParserIndex == GOODBYE_PARSER){
            Debug("Goodbye user");
            return LULU_CS_DONE;
        }
        Debug("User authorized");
//TODO: esta bien esto del selector?
        bool success = SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_READ);
        return success ? LULU_CS_TRANSACTION_READ : LULU_CS_ERROR;
    }

    size_t size;
    byte *ptr = BufferReadPtr(d->WriteBuffer, &size);

    ssize_t bytesWritten = WriteToTcpConnection(connection->ClientTcpConnection, ptr, size);

    if (FUNCTION_ERROR == bytesWritten){
        return LULU_CS_ERROR;
    }

    BufferReadAdv(d->WriteBuffer, bytesWritten);

    return LULU_CS_HELLO_WRITE;

}
