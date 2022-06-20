//
// Created by tluci on 20/6/2022.
//

#include "lulu/fsm_handlers/lulu_transaction.h"
#include "utils/logger.h"
#include "lulu/lulu_messages.h"
#include "lulu/lulu.h"

#include "lulu/lulu_connection.h"
#define T_PARSER_COUNT 9
// TODO: HACER UN ENUM JE
#define T_GOODBYE_PARSER 0
#define T_SET_BUFFER_SIZE_PARSER 1
#define T_METRICS_PARSER 2
#define T_GET_BUFFER_SIZE_PARSER 3
#define T_TIMEOUT_PARSER 4
#define T_SET_USER_PARSER 5
#define T_LIST_USERS_PARSER 6
#define T_DEL_USER_PARSER 7
#define T_GET_TIMEOUT_PARSER 8

int RunTransactionParser(ClientTransactionData *d, byte *buffer, ssize_t bytesRead, LuluConnection *connection, void *data, size_t bufferSize );

#define ATTACHMENT_LULU(key) ( (LuluConnection*)((SelectorKey*)(key))->Data)

void LuluTransactionReadInit(unsigned int state, void *data) {
    LuluConnection *connection = ATTACHMENT_LULU(data);
    ClientTransactionData *d = &connection->Data.Transaction;
    d->ParserIndex = 0;
    d->ReadBuffer = &connection->ReadBuffer;
    d->WriteBuffer = &connection->WriteBuffer;
    ClientDelUserParserReset(&d->DelUserParser);
    ClientGetBufferSizeParserReset(&d->GetBufferSizeParser);
    ClientGetTimeoutParserReset(&d->GetTimeoutParser);
    ClientListUsersParserReset(&d->ListUsersParser);
    ClientMetricsParserReset(&d->MetricsParser);
    ClientSetBufferSizeParserReset(&d->SetBufferSizeParser);
    ClientSetUserParserReset(&d->SetUserParser);
    ClientTimeoutParserReset(&d->TimeoutParser);
    ClientGoodbyeParserReset(&d->GoodbyeParser);
}
void LuluTransactionWriteInit(unsigned int state, void *data) {


}


unsigned LuluTransactionReadRun(void *data) {
    LuluConnection *connection = ATTACHMENT_LULU(data);
    ClientTransactionData *d = &connection->Data.Transaction;
    size_t bufferSize;

    byte *buffer = BufferWritePtr(d->ReadBuffer, &bufferSize);
    ssize_t bytesRead = ReadFromTcpConnection(connection->ClientTcpConnection, buffer, bufferSize);

    if (bytesRead <= 0) {
        Error("Cannot read from Tcp connection");
        return LULU_CS_ERROR;
    }

    BufferWriteAdv(d->ReadBuffer, bytesRead);

    int possibleReturn = NO_RETURN;

    while (d->ParserIndex <= T_PARSER_COUNT ){
        possibleReturn = RunTransactionParser(d, buffer, bytesRead, connection, data, bufferSize);
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
    return LULU_CS_TRANSACTION_WRITE;
}

int RunTransactionParser(ClientTransactionData *d, byte *buffer, ssize_t bytesRead, LuluConnection *connection, void *data, size_t bufferSize ){
    switch (d->ParserIndex) {
        case T_SET_BUFFER_SIZE_PARSER:
            ClientSetBufferSizeParserConsume(&d->SetBufferSizeParser, buffer, bytesRead);

            if (!ClientSetBufferSizeParserHasFinished(d->SetBufferSizeParser.State))
                return LULU_CS_TRANSACTION_READ;

            if (ClientSetBufferSizeParserHasFailed(d->SetBufferSizeParser.State)){
                d->ParserIndex++;
                return NO_RETURN;
            }
            BufferReset(d->ReadBuffer);
            if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_WRITE)) {
                buffer = BufferWritePtr(d->WriteBuffer, &bufferSize);
                size_t bytesWritten = BuildClientSetBufferSizeResponse(buffer, bufferSize, d->SetBufferSizeParser.Value);

                if (0 == bytesWritten)
                    return LULU_CS_ERROR;

                BufferWriteAdv(d->WriteBuffer, (ssize_t ) bytesWritten);

                return LULU_CS_TRANSACTION_WRITE;
            }

            break;
        case T_GOODBYE_PARSER:
            ClientGoodbyeParserConsume(&d->GoodbyeParser, buffer, bytesRead);
//            BufferReset(d->ReadBuffer);
            if (!ClientGoodbyeParserHasFinished(d->GoodbyeParser.State))
                return LULU_CS_TRANSACTION_READ;

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

                return LULU_CS_TRANSACTION_WRITE;
            }
            break;
        case T_DEL_USER_PARSER:
            ClientDelUserParserConsume(&d->DelUserParser, buffer, bytesRead);

            if (!ClientDelUserParserHasFinished(d->DelUserParser.State))
                return LULU_CS_TRANSACTION_READ;

            if (ClientDelUserParserHasFailed(d->DelUserParser.State)){
                d->ParserIndex++;
                return NO_RETURN;
            }
            BufferReset(d->ReadBuffer);
            if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_WRITE)) {
                buffer = BufferWritePtr(d->WriteBuffer, &bufferSize);
                size_t bytesWritten = BuildClientDelUserResponse(buffer, bufferSize, d->DelUserParser.UName);

                if (0 == bytesWritten)
                    return LULU_CS_ERROR;

                BufferWriteAdv(d->WriteBuffer, (ssize_t ) bytesWritten);

                return LULU_CS_TRANSACTION_WRITE;
            }
            break;
        case T_METRICS_PARSER:
            ClientMetricsParserConsume(&d->MetricsParser, buffer, bytesRead);

            if (!ClientMetricsParserHasFinished(d->MetricsParser.State))
                return LULU_CS_TRANSACTION_READ;

            if (ClientMetricsParserHasFailed(d->MetricsParser.State)){
                d->ParserIndex++;
                return NO_RETURN;
            }
            BufferReset(d->ReadBuffer);
            if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_WRITE)) {
                buffer = BufferWritePtr(d->WriteBuffer, &bufferSize);
                size_t bytesWritten = BuildClientGetMetricsResponse(buffer, bufferSize);

                if (0 == bytesWritten)
                    return LULU_CS_ERROR;

                BufferWriteAdv(d->WriteBuffer, (ssize_t ) bytesWritten);

                return LULU_CS_TRANSACTION_WRITE;
            }
            break;
        case T_GET_BUFFER_SIZE_PARSER:
            ClientGetBufferSizeParserConsume(&d->GetBufferSizeParser, buffer, bytesRead);

            if (!ClientGetBufferSizeParserHasFinished(d->GetBufferSizeParser.State))
                return LULU_CS_TRANSACTION_READ;

            if (ClientGetBufferSizeParserHasFailed(d->GetBufferSizeParser.State)){
                d->ParserIndex++;
                return NO_RETURN;
            }
            BufferReset(d->ReadBuffer);
            if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_WRITE)) {
                buffer = BufferWritePtr(d->WriteBuffer, &bufferSize);
                size_t bytesWritten = BuildClientGetBufferSizeResponse(buffer, bufferSize);

                if (0 == bytesWritten)
                    return LULU_CS_ERROR;

                BufferWriteAdv(d->WriteBuffer, (ssize_t ) bytesWritten);

                return LULU_CS_TRANSACTION_WRITE;
            }
            break;
        case T_TIMEOUT_PARSER:
            ClientTimeoutParserConsume(&d->TimeoutParser, buffer, bytesRead);

            if (!ClientTimeoutParserHasFinished(d->TimeoutParser.State))
                return LULU_CS_TRANSACTION_READ;

            if (ClientTimeoutParserHasFailed(d->TimeoutParser.State)){
                d->ParserIndex++;
                return NO_RETURN;
            }
            BufferReset(d->ReadBuffer);
            if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_WRITE)) {
                buffer = BufferWritePtr(d->WriteBuffer, &bufferSize);
                size_t bytesWritten = BuildClientSetTimeoutResponse(buffer, bufferSize, d->TimeoutParser.Value);

                if (0 == bytesWritten)
                    return LULU_CS_ERROR;

                BufferWriteAdv(d->WriteBuffer, (ssize_t ) bytesWritten);

                return LULU_CS_TRANSACTION_WRITE;
            }
            break;
        case T_SET_USER_PARSER:
            ClientSetUserParserConsume(&d->SetUserParser, buffer, bytesRead);

            if (!ClientSetUserParserHasFinished(d->SetUserParser.State))
                return LULU_CS_TRANSACTION_READ;

            if (ClientSetUserParserHasFailed(d->SetUserParser.State)){
                d->ParserIndex++;
                return NO_RETURN;
            }
            BufferReset(d->ReadBuffer);
            if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_WRITE)) {
                buffer = BufferWritePtr(d->WriteBuffer, &bufferSize);
                size_t bytesWritten = BuildClientSetUserResponse(buffer, bufferSize, d->SetUserParser.UName,d->SetUserParser.Passwd );

                if (0 == bytesWritten)
                    return LULU_CS_ERROR;

                BufferWriteAdv(d->WriteBuffer, (ssize_t ) bytesWritten);

                return LULU_CS_TRANSACTION_WRITE;
            }
            break;
        case T_LIST_USERS_PARSER:
            ClientListUsersParserConsume(&d->ListUsersParser, buffer, bytesRead);

            if (!ClientListUsersParserHasFinished(d->ListUsersParser.State))
                return LULU_CS_TRANSACTION_READ;

            if (ClientListUsersParserHasFailed(d->ListUsersParser.State)){
                d->ParserIndex++;
                return NO_RETURN;
            }
            BufferReset(d->ReadBuffer);
            if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_WRITE)) {
                buffer = BufferWritePtr(d->WriteBuffer, &bufferSize);
                size_t bytesWritten = BuildClientListUsersResponse(buffer, bufferSize );

                if (0 == bytesWritten)
                    return LULU_CS_ERROR;

                BufferWriteAdv(d->WriteBuffer, (ssize_t ) bytesWritten);

                return LULU_CS_TRANSACTION_WRITE;
            }
            break;
        case T_GET_TIMEOUT_PARSER:
            ClientGetTimeoutParserConsume(&d->GetTimeoutParser, buffer, bytesRead);

            if (!ClientGetTimeoutParserHasFinished(d->GetTimeoutParser.State))
                return LULU_CS_TRANSACTION_READ;

            if (ClientGetTimeoutParserHasFailed(d->GetTimeoutParser.State)){
                d->ParserIndex++;
                return NO_RETURN;
            }
            BufferReset(d->ReadBuffer);
            if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_WRITE)) {
                buffer = BufferWritePtr(d->WriteBuffer, &bufferSize);
                size_t bytesWritten = BuildClientGetTimeoutResponse(buffer, bufferSize );

                if (0 == bytesWritten)
                    return LULU_CS_ERROR;

                BufferWriteAdv(d->WriteBuffer, (ssize_t ) bytesWritten);

                return LULU_CS_TRANSACTION_WRITE;
            }
            break;
        case T_PARSER_COUNT:
            BufferReset(d->ReadBuffer);
            if (SELECTOR_STATUS_SUCCESS == SelectorSetInterestKey(data, SELECTOR_OP_WRITE)) {
                buffer = BufferWritePtr(d->WriteBuffer, &bufferSize);
                size_t bytesWritten = BuildClientNotRecognisedResponse(buffer, bufferSize);

                if (0 == bytesWritten)
                    return LULU_CS_ERROR;

                BufferWriteAdv(d->WriteBuffer, (ssize_t ) bytesWritten);

                return LULU_CS_TRANSACTION_WRITE;
            }
            return LULU_CS_ERROR;
        default:
            Error("hay un num muy grande");
            break;
    }
    return LULU_CS_ERROR;
}

void LuluTransactionReadClose(unsigned int state, void *data) {

}
void LuluTransactionWriteClose(unsigned int state, void *data) {
    LuluConnection *connection = ATTACHMENT_LULU(data);
    ClientTransactionData *d = &connection->Data.Transaction;
    d->ParserIndex = 0;
    BufferReset(d->WriteBuffer);
    ClientDelUserParserReset(&d->DelUserParser);
    ClientGetBufferSizeParserReset(&d->GetBufferSizeParser);
    ClientGetTimeoutParserReset(&d->GetTimeoutParser);
    ClientListUsersParserReset(&d->ListUsersParser);
    ClientMetricsParserReset(&d->MetricsParser);
    ClientSetBufferSizeParserReset(&d->SetBufferSizeParser);
    ClientSetUserParserReset(&d->SetUserParser);
    ClientTimeoutParserReset(&d->TimeoutParser);
    ClientGoodbyeParserReset(&d->GoodbyeParser);
}

unsigned LuluTransactionWriteRun(void *data) {
    LuluConnection *connection = ATTACHMENT_LULU(data);
    ClientTransactionData *d = &connection->Data.Transaction;

    if (!BufferCanRead(d->WriteBuffer)) {
        if(d->ParserIndex == T_GOODBYE_PARSER){
            Debug("Goodbye user");
            return LULU_CS_DONE;
        }

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

    return LULU_CS_TRANSACTION_WRITE;

}
