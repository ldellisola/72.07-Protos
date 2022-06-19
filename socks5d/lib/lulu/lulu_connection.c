//
// Created by tluci on 19/6/2022.
//

#include "lulu/lulu_connection.h"
#include "lulu/lulu_connection_status.h"
#include "utils/utils.h"
#include "utils/logger.h"
#include <assert.h>
static void LuluConnectionRead(SelectorKey *key);
static void LuluConnectionWrite(SelectorKey *key);
//static void LuluConnectionBlock(SelectorKey *key);
//
const FdHandler luluConnectionHandler = {
        .handle_read   = LuluConnectionRead,
        .handle_write  = LuluConnectionWrite,
//        .handle_block  = LuluConnectionBlock,
};

static StateDefinition luluConnectionFsm[] = {
        {
                .state = LULU_CS_AUTH_READ,
//                .on_arrival = AuthReadInit,
//                .on_departure = AuthReadClose,
//                .on_read_ready = AuthReadRun
        },
        {
                .state = LULU_CS_AUTH_WRITE,
//                .on_write_ready = AuthWriteRun,
//                .on_departure = AuthWriteClose
        },
        {
                .state = LULU_CS_TRANSACTION_READ,
//                .on_arrival = RequestReadInit,
//                .on_departure = RequestReadClose,
//                .on_read_ready = RequestReadRun
        },
        {
                .state = LULU_CS_TRANSACTION_WRITE,
//                .on_arrival = RequestWriteInit,
//                .on_write_ready = RequestWriteRun,
//                .on_departure = RequestWriteClose
        },
        {
                .state = LULU_CS_DONE,
        },
        {
                .state = LULU_CS_ERROR,
        },
};
typedef enum {
    PooledLuluConnectionEmpty,
    PooledLuluConnectionInUse
} PooledLuluConnectionStatus;

typedef struct PooledLuluConnection_{
    struct PooledLuluConnection_ * Next;
    LuluConnection Connection;
    PooledLuluConnectionStatus Status;
}PooledLuluConnection;

PooledLuluConnection * luluPool;

LuluConnection *CreateLuluConnection(TcpConnection *tcpConnection) {
    Info("Creating LuluConnection.");

    LuluConnection *connection = GetLuluConnection();

    if (null == tcpConnection)
        LogError(false, "Cannot allocate space for LuluConnection");

    connection->ClientTcpConnection = tcpConnection;
    connection->Handler = &luluConnectionHandler;
    connection->User = null;
    connection->Fsm.InitialState = LULU_CS_AUTH_READ;
    connection->Fsm.StatesSize = LULU_CS_ERROR;
    InitFsm(&connection->Fsm, luluConnectionFsm);

    void *readBuffer = calloc(1000, sizeof(byte));
    void *writeBuffer = calloc(1000, sizeof(byte));
    BufferInit(&connection->WriteBuffer, 1000, writeBuffer);
    BufferInit(&connection->ReadBuffer, 1000, readBuffer);

    Info("LuluConnection Created!");
    return connection;
}

LuluConnection *GetLuluConnection() {
    if (null == luluPool) {
        LogError(false, "LULU pool was not initialized");
        return null;
    }

    PooledLuluConnection * temp;
    for ( temp = luluPool; temp != null ; temp = temp->Next) {
        if (PooledLuluConnectionEmpty == temp->Status) {
            temp->Status = PooledLuluConnectionInUse;
            return &temp->Connection;
        }

        if (null == temp->Next)
            break;
    }

    temp->Next = calloc(1, sizeof(PooledLuluConnection));
    temp = temp->Next;
    temp->Status = PooledLuluConnectionInUse;

    return &temp->Connection;
}

void CreateLuluConnectionPool(int initialSize) {
    Debug("Initializing LULU Pool");
    if (initialSize < 1) {
        LogInfo("Invalid initial pool size %d, using default value 1", initialSize);
        initialSize = 1;
    }

    luluPool = calloc(1, sizeof(PooledLuluConnection));
    luluPool->Status = PooledLuluConnectionEmpty;
    PooledLuluConnection *current = luluPool;

    while (--initialSize > 0) {
        current->Next = calloc(1, sizeof(PooledLuluConnection));
        current = current->Next;
        current->Status = PooledLuluConnectionEmpty;
    }
}

void DisposeLuluConnection(LuluConnection *connection, fd_selector selector) {
    Info("Disposing LuluConnection...");
    if (null == connection) {
        LogError(false, "Cannot destroy NULL Socks5Connection");
        return;
    }

    if (null != connection->ClientTcpConnection)
        DisposeTcpConnection(connection->ClientTcpConnection, selector);

    if (null != connection->ReadBuffer.Data)
        free(connection->ReadBuffer.Data);

    if (null != connection->WriteBuffer.Data)
        free(connection->WriteBuffer.Data);

//    if (null != connection->User)
//        LogOutLuluUser(connection->User);

    DestroyLuluConnection(connection);
    Info("Socks5Connection disposed!");
}
void DestroyLuluConnection(LuluConnection *connection) {
    if (null == luluPool) {
        LogError(false, "TCP pool was not initialized");
        return;
    }

    memset(connection,0, sizeof(LuluConnection));

    PooledLuluConnection * temp;
    for (temp = luluPool; temp != null && &temp->Connection != connection ; temp = temp->Next);

    if (null == temp)
    {
        LogError(false,"Error while destroying connection!");
        return;
    }
    assert(&temp->Connection == connection);
    temp->Status = PooledLuluConnectionEmpty;

}

#define ATTACHMENT(key) ( (LuluConnection*)((SelectorKey*)(key))->Data)

void LuluConnectionRead(SelectorKey *key) {
    FiniteStateMachine *fsm = &ATTACHMENT(key)->Fsm;
    CONNECTION_STATE st = HandleReadFsm(fsm, key);

    if (LULU_CS_ERROR == st || LULU_CS_DONE == st) {
        DisposeLuluConnection(ATTACHMENT(key), key->Selector);
    }
}


void LuluConnectionWrite(SelectorKey *key) {
    FiniteStateMachine *fsm = &ATTACHMENT(key)->Fsm;
    CONNECTION_STATE st = HandleWriteFsm(fsm, key);

    if (LULU_CS_ERROR == st || LULU_CS_DONE == st) {
        DisposeLuluConnection(ATTACHMENT(key), key->Selector);
    }
}

