//
// Created by tluci on 19/6/2022.
//

#include "lulu/fsm_handlers/lulu_hello.h"
#include "lulu/lulu_connection.h"
#include "utils/logger.h"
#include "lulu/lulu_messages.h"

int RunParser(ClientHelloData *d, byte *buffer, ssize_t bytesRead, LuluConnection *connection, void *data, size_t bufferSize );

#define ATTACHMENT(key) ( (LuluConnection*)((SelectorKey*)(key))->Data)
//I am called once
void LuluHelloReadInit(unsigned int state, void *data) {
    LuluConnection *connection = ATTACHMENT(data);
    ClientHelloData *d = &connection->Auth;
    d->ParserIndex = 0;
    d->ClientHelloSucceeded = false;
    d->ReadBuffer = &connection->ReadBuffer;
    d->WriteBuffer = &connection->WriteBuffer;
    ClientHelloParserReset(&d->HelloParser);
    ClientGoodbyeParserReset(&d->GoodbyeParser);
}
unsigned LuluHelloReadRun(void *data) {
    LuluConnection *connection = ATTACHMENT(data);
    ClientHelloData *d = &connection->Auth;
    size_t bufferSize;

    byte *buffer = BufferWritePtr(d->ReadBuffer, &bufferSize);
    ssize_t bytesRead = ReadFromTcpConnection(connection->ClientTcpConnection, buffer, bufferSize);

    if (bytesRead <= 0) {
        Error("Cannot read from Tcp connection");
        return LULU_CS_ERROR;
    }

    BufferWriteAdv(d->ReadBuffer, bytesRead);

    int possibleReturn = NO_RETURN;

    while (d->ParserIndex < PARSER_COUNT ){
         possibleReturn = RunParser(d, buffer, bytesRead, connection, data, bufferSize);
         if(possibleReturn != NO_RETURN){
             return possibleReturn;
         }
    }


    return LULU_CS_ERROR;
}

int RunParser(ClientHelloData *d, byte *buffer, ssize_t bytesRead, LuluConnection *connection, void *data, size_t bufferSize ){
    switch (d->ParserIndex) {
        case 0:
            ClientHelloParserConsume(&d->HelloParser, buffer, bytesRead);

            if (!ClientHelloParserHasFinished(d->HelloParser.State))
                return LULU_CS_HELLO_READ;

            if (ClientHelloParserHasFailed(d->HelloParser.State)){
                d->ParserIndex++;
                return NO_RETURN;
            }
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
        case 1:
            ClientGoodbyeParserConsume(&d->GoodbyeParser, buffer, bytesRead);

            if (!ClientGoodbyeParserHasFinished(d->GoodbyeParser.State))
                return LULU_CS_HELLO_READ;

            if (ClientGoodbyeParserHasFailed(d->GoodbyeParser.State)){
                d->ParserIndex++;
                return LULU_CS_ERROR;
            }
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
            return LULU_CS_ERROR;
    }
    return LULU_CS_ERROR;
}

