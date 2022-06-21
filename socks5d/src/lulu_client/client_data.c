//
// Created by Lucas Dell'Isola on 20/06/2022.
//

#include "lulu_client/client_data.h"

#include "tcp/tcp_connection.h"
#include "utils/utils.h"
#include "fsm/fsm.h"
#include "lulu_client/fsm/client_auth.h"
#include "lulu_client/fsm/client_wait_connection.h"
#include "tcp/tcp.h"
#include "lulu_client/fsm/client_transaction.h"
#include "lulu_client/fsm/client_timeout.h"
#include "lulu_client/fsm/client_buffersize.h"
#include "lulu_client/fsm/client_metrics.h"
#include "lulu_client/fsm/client_users.h"
#include "lulu_client/fsm/client_goodbye.h"

#define ATTACHMENT(key) ( (ClientData *)((SelectorKey*)(key))->Data)


static void ClientConnectionRead(SelectorKey *key);
static void ClientConnectionWrite(SelectorKey *key);
static void ClientConnectionBlock(SelectorKey *key);

static FdHandler handler = {
        .handle_write = ClientConnectionWrite,
        .handle_block = ClientConnectionBlock,
        .handle_close = null,
        .handle_read = ClientConnectionRead
};


static StateDefinition clientConnectionFsm[] = {
        {
            .state = CS_WAITING_CONNECTION,
            .on_read_ready = LuluClientConnect,
            .on_write_ready = LuluClientConnect,
        },
        {
                .state = CS_WRITE_AUTH,
                .on_arrival = LuluClientAuthWriteInit,
                .on_write_ready = LuluClientAuthWriteRun,
                .on_departure = LuluClientAuthWriteStop
        },
        {
                .state = CS_READ_AUTH,
                .on_read_ready = LuluClientReadRun
        },
        {
                .state = CS_TRANSACTION,
                .on_arrival = LuluClientTransactionInit,
                .on_read_ready = LuluClientTransactionRead
        },
        {
                .state = CS_GET_TIMEOUT_WRITE,
                .on_arrival = LuluClientGetTimeoutWriteInit,
                .on_write_ready = LuluClientGetTimeoutWriteRun
        },
        {
                .state = CS_GET_TIMEOUT_READ,
                .on_arrival = LuluClientGetTimeoutReadInit,
                .on_read_ready = LuluClientGetTimeoutReadRun,
                .on_departure = LuluClientGetTimeoutReadStop
        },
        {
                .state = CS_SET_TIMEOUT_READ_LOCAL,
                .on_arrival = LuluClientSetTimeoutReadLocalInit,
                .on_read_ready = LuluClientSetTimeoutReadLocalRun
        },
        {
                .state = CS_SET_TIMEOUT_WRITE_REMOTE,
                .on_arrival = LuluClientSetTimeoutWriteRemoteInit,
                .on_write_ready =LuluClientSetTimeoutWriteRemoteRun
        },
        {
                .state = CS_SET_TIMEOUT_READ_REMOTE,
                .on_arrival = LuluClientSetTimeoutReadRemoteInit,
                .on_read_ready = LuluClientSetTimeoutReadRemoteRun,
                .on_departure = LuluClientSetTimeoutReadRemoteStop
        },
        {
                .state = CS_GET_BUFFERSIZE_WRITE,
                .on_arrival = LuluClientGetBufferSizeWriteInit,
                .on_write_ready = LuluClientGetBufferSizeWriteRun
        },
        {
                .state = CS_GET_BUFFERSIZE_READ,
                .on_arrival = LuluClientGetBufferSizeReadInit,
                .on_read_ready = LuluClientGetBufferSizeReadRun,
                .on_departure = LuluClientGetBufferSizeReadStop
        },
        {
                .state = CS_SET_BUFFERSIZE_READ_LOCAL,
                .on_arrival = LuluClientSetBufferSizeReadLocalInit,
                .on_read_ready = LuluClientSetBufferSizeReadLocalRun
        },
        {
                .state = CS_SET_BUFFERSIZE_WRITE_REMOTE,
                .on_arrival = LuluClientSetBufferSizeWriteRemoteInit,
                .on_write_ready =LuluClientSetBufferSizeWriteRemoteRun
        },
        {
                .state = CS_SET_BUFFERSIZE_READ_REMOTE,
                .on_arrival = LuluClientSetBufferSizeReadRemoteInit,
                .on_read_ready = LuluClientSetBufferSizeReadRemoteRun,
                .on_departure = LuluClientSetBufferSizeReadRemoteStop
        },
        {
                .state = CS_METRICS_WRITE_REMOTE,
                .on_arrival = LuluClientMetricsWriteRemoteInit,
                .on_write_ready = LuluClientMetricsWriteRemoteRun,
        },
        {
                .state = CS_METRICS_READ_REMOTE,
                .on_arrival = LuluClientMetricsReadRemoteInit,
                .on_read_ready = LuluClientMetricsReadRemoteRun,
                .on_departure = LuluClientMetricsReadRemoteStop
        },
        {
                .state = CS_SET_USER_READ_LOCAL,
                .on_arrival = LuluClientSetUserReadLocalInit,
                .on_read_ready = LuluClientSetUserReadLocalRun
            },
        {
                .state = CS_SET_USER_WRITE_REMOTE,
                .on_arrival = LuluClientSetUserWriteRemoteInit,
                .on_write_ready =LuluClientSetUserWriteRemoteRun
            },
        {
                .state =CS_SET_USER_READ_REMOTE,
                .on_arrival = LuluClientSetUserReadRemoteInit,
                .on_read_ready = LuluClientSetUserReadRemoteRun,
                .on_departure = LuluClientSetUserReadRemoteStop
            },
        {
                .state = CS_DEL_USER_READ_LOCAL,
                .on_arrival = LuluClientDelUserReadLocalInit,
                .on_read_ready = LuluClientDelUserReadLocalRun
            },
        {
                .state = CS_DEL_USER_WRITE_REMOTE,
                .on_arrival = LuluClientDelUserWriteRemoteInit,
                .on_write_ready =LuluClientDelUserWriteRemoteRun
            },
        {
                .state =CS_DEL_USER_READ_REMOTE,
                .on_arrival = LuluClientDelUserReadRemoteInit,
                .on_read_ready = LuluClientDelUserReadRemoteRun,
                .on_departure = LuluClientDelUserReadRemoteStop
            },
        {
                .state = CS_LIST_USER_WRITE_REMOTE,
                .on_arrival = LuluClientListUserWriteRemoteInit,
                .on_write_ready = LuluClientListUserWriteRemoteRun
            },
        {
                .state = CS_LIST_USER_READ_REMOTE,
                .on_arrival = LuluClientListUserReadInit,
                .on_read_ready = LuluClientListUserReadRun
            },
        {
                .state = CS_GOODBYE_WRITE_REMOTE,
                .on_arrival = LuluClientGoodbyeWriteRemoteInit,
                .on_write_ready = LuluClientGoodbyeWriteRemoteRun
        },
        {
                .state = CS_GOODBYE_READ_REMOTE,
                .on_arrival = LuluClientGoodbyeReadInit,
                .on_read_ready = LuluClientGoodbyeReadRun,
                .on_departure = LuluClientGoodbyeReadStop
        },
        {
                .state = CS_DONE,
        },
        {
                .state = CS_ERROR,
        },
};

FdHandler *GetClientHandlers() {
    return &handler;
}

void InitClient(ClientData *data) {
    data->Fsm.InitialState = CS_WAITING_CONNECTION;
    data->Fsm.StatesSize = CS_ERROR;
    InitFsm(&data->Fsm, clientConnectionFsm);

    BufferInit(&data->Buffer,1000, calloc(1000, sizeof(char)));
}


void ClientConnectionRead(SelectorKey *key) {
    FiniteStateMachine *fsm = &ATTACHMENT(key)->Fsm;
    ClientState st = HandleReadFsm(fsm, key);
    ClientData  *  d = ATTACHMENT(key);

    if (CS_ERROR == st || CS_DONE == st) {
        SelectorSetInterest(key->Selector,0,SELECTOR_OP_NOOP);
        if (null != d && null != d->Remote)
            SelectorSetInterest(key->Selector,d->Remote->FileDescriptor,SELECTOR_OP_NOOP);
        StopTcpServer();
    }
}

void ClientConnectionWrite(SelectorKey *key) {
    FiniteStateMachine *fsm = &ATTACHMENT(key)->Fsm;
    ClientState st = HandleWriteFsm(fsm, key);
    ClientData  *  d = ATTACHMENT(key);

    if (CS_ERROR == st || CS_DONE == st) {
        SelectorSetInterest(key->Selector,0,SELECTOR_OP_NOOP);
        if (null != d && null != d->Remote)
            SelectorSetInterest(key->Selector,d->Remote->FileDescriptor,SELECTOR_OP_NOOP);
        StopTcpServer();
    }
}

void ClientConnectionBlock(SelectorKey *key) {
    FiniteStateMachine *fsm = &ATTACHMENT(key)->Fsm;
    ClientData  *  d = ATTACHMENT(key);
    ClientState st = HandleBlockFsm(fsm, key);

    if (CS_ERROR == st || CS_DONE == st) {
        SelectorSetInterest(key->Selector,0,SELECTOR_OP_NOOP);
        if (null != d && null != d->Remote)
            SelectorSetInterest(key->Selector,d->Remote->FileDescriptor,SELECTOR_OP_NOOP);
        StopTcpServer();
    }
}
