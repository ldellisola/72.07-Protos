//
// Created by Lucas Dell'Isola on 20/06/2022.
//

#ifndef SOCKS5D_CLIENT_DATA_H
#define SOCKS5D_CLIENT_DATA_H

#include "tcp/tcp_connection.h"
#include "fsm/fsm.h"
#include "utils/buffer.h"

typedef enum {
    CS_WAITING_CONNECTION,
    CS_WRITE_AUTH,
    CS_READ_AUTH,
    CS_TRANSACTION,
    CS_GET_TIMEOUT_WRITE,
    CS_GET_TIMEOUT_READ,
    CS_SET_TIMEOUT_READ_LOCAL,
    CS_SET_TIMEOUT_WRITE_REMOTE,
    CS_SET_TIMEOUT_READ_REMOTE,
    CS_GET_BUFFERSIZE_WRITE,
    CS_GET_BUFFERSIZE_READ,
    CS_SET_BUFFERSIZE_READ_LOCAL,
    CS_SET_BUFFERSIZE_WRITE_REMOTE,
    CS_SET_BUFFERSIZE_READ_REMOTE,
    CS_METRICS_WRITE_REMOTE,
    CS_METRICS_READ_REMOTE,
    CS_SET_USER_READ_LOCAL,
    CS_SET_USER_WRITE_REMOTE,
    CS_SET_USER_READ_REMOTE,
    CS_DEL_USER_READ_LOCAL,
    CS_DEL_USER_WRITE_REMOTE,
    CS_DEL_USER_READ_REMOTE,
    CS_LIST_USER_WRITE_REMOTE,
    CS_LIST_USER_READ_REMOTE,
    CS_GOODBYE_WRITE_REMOTE,
    CS_GOODBYE_READ_REMOTE,
    CS_DONE,
    CS_ERROR
}ClientState;

typedef struct {
    TcpConnection * Remote;
    FiniteStateMachine Fsm;
    void * Data;
    ArrayBuffer Buffer;
    const char * Username, * Password;
}ClientData;

void InitClient(ClientData * data);

FdHandler * GetClientHandlers();

#endif //SOCKS5D_CLIENT_DATA_H
