//
// Created by tluci on 19/6/2022.
//

#include "lulu/lulu_connection.h"
#include "lulu/fsm_handlers/lulu_hello.h"
#include "lulu/lulu_connection_status.h"
#include "utils/utils.h"
#include "utils/logger.h"
#include "utils/object_pool.h"

static void LuluConnectionRead(SelectorKey *key);
static void LuluConnectionWrite(SelectorKey *key);
static void DestroyLuluConnection(void *data);

const FdHandler luluConnectionHandler = {
        .handle_read   = LuluConnectionRead,
        .handle_write  = LuluConnectionWrite
};

static StateDefinition luluConnectionFsm[] = {
        {
                .state = LULU_CS_HELLO_READ,
                .on_arrival = LuluHelloReadInit,
                .on_departure = LuluHelloReadClose,
                .on_read_ready = LuluHelloReadRun
        },
        {
                .state = LULU_CS_HELLO_WRITE,
                .on_write_ready = LuluHelloWriteRun,
                .on_departure = LuluHelloWriteClose
        },
        {
                .state = LULU_CS_TRANSACTION_READ,
                .on_arrival = LuluTransactionReadInit,
                .on_departure = LuluTransactionReadClose,
                .on_read_ready = LuluTransactionReadRun
        },
        {
                .state = LULU_CS_TRANSACTION_WRITE,
                .on_arrival = LuluTransactionWriteInit,
                .on_write_ready = LuluTransactionWriteRun,
                .on_departure = LuluTransactionWriteClose
        },
        {
                .state = LULU_CS_DONE,
        },
        {
                .state = LULU_CS_ERROR,
        },
};

static ObjectPool luluPool;

LuluConnection *CreateLuluConnection(TcpConnection *tcpConnection) {
    Debug("Creating LuluConnection.");

    LuluConnection *connection = GetLuluConnection();

    if (null == tcpConnection)
        Fatal("Cannot allocate space for LuluConnection");

    connection->ClientTcpConnection = tcpConnection;
    connection->Handler = &luluConnectionHandler;
    connection->User = null;
    connection->Fsm.InitialState = LULU_CS_HELLO_READ;
    connection->Fsm.StatesSize = LULU_CS_ERROR;
    InitFsm(&connection->Fsm, luluConnectionFsm);

    void *readBuffer = calloc(1000, sizeof(byte));
    void *writeBuffer = calloc(1000, sizeof(byte));
    BufferInit(&connection->WriteBuffer, 1000, writeBuffer);
    BufferInit(&connection->ReadBuffer, 1000, readBuffer);

    Debug("LuluConnection Created!");
    return connection;
}

LuluConnection *GetLuluConnection() {

    return GetObjectFromPool(&luluPool);

}

static  ObjectPoolHandlers luluPoolHandlers = {
    .OnDispose = DestroyLuluConnection
};

void CreateLuluConnectionPool(int initialSize) {
    Debug("Initializing LULU Pool");

    InitObjectPool(&luluPool,&luluPoolHandlers,initialSize, sizeof(LuluConnection));

}


void DisposeLuluConnection(LuluConnection *connection, fd_selector selector) {
    Debug("Disposing LuluConnection...");
    if (null == connection) {
        Warning("Cannot destroy NULL Socks5Connection");
        return;
    }

    if (null != connection->ClientTcpConnection)
        DisposeTcpConnection(connection->ClientTcpConnection, selector);

    if (null != connection->ReadBuffer.Data)
        free(connection->ReadBuffer.Data);

    if (null != connection->WriteBuffer.Data)
        free(connection->WriteBuffer.Data);

    if (null != connection->User)
        LogOutLuluUser(connection->User);

    DestroyObject(&luluPool,connection);
    Debug("Socks5Connection disposed!");
}
void DestroyLuluConnection(void *data) {
    if (null == data) {
        Warning("Invalid data");
        return;
    }
    memset(data,0, sizeof(LuluConnection));
}

#define ATTACHMENT(key) ( (LuluConnection*)((SelectorKey*)(key))->Data)

void LuluConnectionRead(SelectorKey *key) {
    FiniteStateMachine *fsm = &ATTACHMENT(key)->Fsm;
    LULU_CONNECTION_STATE st = HandleReadFsm(fsm, key);

    if (LULU_CS_ERROR == st || LULU_CS_DONE == st) {
        DisposeLuluConnection(ATTACHMENT(key), key->Selector);
    }
}


void LuluConnectionWrite(SelectorKey *key) {
    FiniteStateMachine *fsm = &ATTACHMENT(key)->Fsm;
    LULU_CONNECTION_STATE st = HandleWriteFsm(fsm, key);

    if (LULU_CS_ERROR == st || LULU_CS_DONE == st) {
        DisposeLuluConnection(ATTACHMENT(key), key->Selector);
    }
}

void CleanLuluPool(){
    CleanObjectPool(&luluPool);
}
