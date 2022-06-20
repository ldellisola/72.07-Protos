//
// Created by tluci on 20/6/2022.
//

#ifndef SOCKS5D_LULU_TRANSACTION_H
#define SOCKS5D_LULU_TRANSACTION_H
#include "utils/utils.h"
#include "parsers/lulu/client_set_buffersize_parser.h"
#include "parsers/lulu/client_metrics_parser.h"
#include "parsers/lulu/client_get_buffersize_parser.h"
#include "parsers/lulu/client_goodbye_parser.h"
#include "parsers/lulu/client_timeout_parser.h"
#include "parsers/lulu/client_set_user_parser.h"
#include "parsers/lulu/client_list_users_parser.h"
#include "parsers/lulu/client_del_user_parser.h"
#include "parsers/lulu/client_get_timeout_parser.h"
#include "utils/buffer.h"



typedef struct {
    byte ParserIndex;
    ClientGoodbyeParser GoodbyeParser;
    ClientSetBufferSizeParser SetBufferSizeParser;
    ClientMetricsParser MetricsParser;
    ClientGetBufferSizeParser GetBufferSizeParser;
    ClientTimeoutParser TimeoutParser;
    ClientSetUserParser SetUserParser;
    ClientListUsersParser ListUsersParser;
    ClientDelUserParser DelUserParser;
    ClientGetTimeoutParser GetTimeoutParser;
    ArrayBuffer *WriteBuffer, *ReadBuffer;
} ClientTransactionData;


void LuluTransactionReadInit(unsigned int state, void *data);
unsigned LuluTransactionReadRun(void *data);
unsigned LuluTransactionWriteRun(void *data);
void LuluTransactionWriteClose(unsigned int state, void *data);
void LuluTransactionReadClose(unsigned int state, void *data);
void LuluTransactionWriteInit(unsigned int state, void *data);
#endif //SOCKS5D_LULU_TRANSACTION_H
