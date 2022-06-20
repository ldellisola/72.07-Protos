//
// Created by tluci on 19/6/2022.
//

#ifndef SOCKS5D_LULU_CONNECTION_H
#define SOCKS5D_LULU_CONNECTION_H
#include "lulu/lulu_server.h"
#include "utils/buffer.h"
#include "parsers/lulu/client_del_user_parser.h"
#include "parsers/lulu/client_list_users_parser.h"
#include "parsers/lulu/client_set_user_parser.h"
#include "parsers/lulu/client_get_timeout_parser.h"
#include "parsers/lulu/client_timeout_parser.h"
#include "parsers/lulu/client_goodbye_parser.h"
#include "parsers/lulu/client_hello_parser.h"
#include "parsers/lulu/client_get_buffersize_parser.h"
#include "parsers/lulu/client_metrics_parser.h"
#include "parsers/lulu/client_set_buffersize_parser.h"
#include "lulu/lulu_users.h"
#include "fsm/fsm.h"
#include "lulu/fsm_handlers/lulu_hello.h"
#include "lulu/fsm_handlers/lulu_transaction.h"
#include "tcp/tcp.h"

typedef struct {
    time_t LastConnectionOn;
    FiniteStateMachine Fsm;
    TcpConnection *ClientTcpConnection;

    struct LuluUser * User;
    const FdHandler *Handler;
    union {
        ClientHelloData Auth;
        ClientTransactionData Transaction;
    } Data;

    ClientDelUserParser DelUserParser;
    ClientGetBufferSizeParser GetBufferSizeParser;
    ClientGetTimeoutParser GetTimeoutParser;
    ClientGoodbyeParser GoodbyeParser;
    ClientHelloParser HelloParser;
    ClientListUsersParser ListUsersParser;
    ClientMetricsParser MetricsParser;
    ClientSetBufferSizeParser SetBufferSizeParser;
    ClientSetUserParser SetUserParser;
    ClientTimeoutParser TimeoutParser;
    ArrayBuffer ReadBuffer, WriteBuffer;
} LuluConnection;

LuluConnection *CreateLuluConnection(TcpConnection *tcpConnection);
LuluConnection *GetLuluConnection();
void CreateLuluConnectionPool(int initialSize);
void DisposeLuluConnection(LuluConnection *connection, fd_selector selector);
void DestroyLuluConnection(LuluConnection *connection);

#endif //SOCKS5D_LULU_CONNECTION_H
