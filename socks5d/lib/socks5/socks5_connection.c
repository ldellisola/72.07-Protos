//
// Created by Lucas Dell'Isola on 06/06/2022.
//
#include "socks5/socks5_connection.h"
#include <string.h>
#include <assert.h>
#include <time.h>
#include "socks5/fsm_handlers/socks5_hello.h"
#include "utils/logger.h"
#include "parsers/auth_parser.h"
#include "fsm/fsm.h"
#include "socks5/fsm_handlers/socks5_auth.h"
#include "socks5/fsm_handlers/socks5_request.h"
#include "socks5/fsm_handlers/socks5_establish_connection.h"
#include "socks5/fsm_handlers/socks5_connected.h"
#include "socks5/fsm_handlers/socks5_client.h"
#include "socks5/fsm_handlers/socks5_remote.h"
#include "socks5/fsm_handlers/socks5_dns.h"
#include "socks5/socks5_metrics.h"

// TODO: Test

static void Socks5ConnectionRead(SelectorKey *key);
static void Socks5ConnectionWrite(SelectorKey *key);
static void Socks5ConnectionBlock(SelectorKey *key);

const FdHandler socks5ConnectionHandler = {
        .handle_read   = Socks5ConnectionRead,
        .handle_write  = Socks5ConnectionWrite,
        .handle_block  = Socks5ConnectionBlock,
};

static StateDefinition socks5ConnectionFsm[] = {
        {
                .state = CS_HELLO_READ,
                .on_arrival = HelloReadInit,
                .on_departure = HelloReadClose,
                .on_read_ready = HelloReadRun
        },
        {
                .state = CS_HELLO_WRITE,
                .on_write_ready = HelloWriteRun,
                .on_departure = HelloWriteClose
        },
        {
                .state = CS_AUTH_READ,
                .on_arrival = AuthReadInit,
                .on_departure = AuthReadClose,
                .on_read_ready = AuthReadRun
        },
        {
                .state = CS_AUTH_WRITE,
                .on_write_ready = AuthWriteRun,
                .on_departure = AuthWriteClose
        },
        {
                .state = CS_REQUEST_READ,
                .on_arrival = RequestReadInit,
                .on_departure = RequestReadClose,
                .on_read_ready = RequestReadRun
        },
        {
                .state = CS_REQUEST_WRITE,
                .on_arrival = RequestWriteInit,
                .on_write_ready = RequestWriteRun,
                .on_departure = RequestWriteClose
        },
        {
                .state = CS_ESTABLISH_CONNECTION,
                .on_arrival = EstablishConnectionInit,
                .on_reentry = EstablishConnectionInit,
                .on_write_ready = EstablishConnectionRun,
                .on_read_ready = EstablishConnectionRun,
                .on_departure = EstablishConnectionClose
        },
        {
                .state = CS_CONNECTED,
                .on_read_ready = ConnectedConnectionRun,
        },
        {
                .state = CS_CLIENT_READ,
                .on_arrival = ClientReadInit,
                .on_read_ready = ClientReadRun,
                .on_departure = ClientReadClose
        },
        {
                .state = CS_REMOTE_WRITE,
                .on_arrival = RemoteWriteInit,
                .on_write_ready = RemoteWriteRun,
                .on_departure = RemoteWriteClose
        },
        {
                .state = CS_REMOTE_READ,
                .on_arrival = RemoteReadInit,
                .on_read_ready = RemoteReadRun,
                .on_departure = RemoteReadClose
        },
        {
                .state = CS_CLIENT_WRITE,
                .on_arrival = ClientWriteInit,
                .on_write_ready = ClientWriteRun,
                .on_departure = ClientWriteClose
        },
        {
            .state = CS_DNS_READ,
            .on_block_ready = DnsRead
            },
        {
                .state = CS_DONE,
        },
        {
                .state = CS_ERROR,
        },
};

typedef enum {
    PooledSocks5ConnectionEmpty,
    PooledSocks5ConnectionInUse
} PooledSocks5ConnectionStatus;

typedef struct PooledSocks5Connection_{
    struct PooledSocks5Connection_ * Next;
    Socks5Connection Connection;
    PooledSocks5ConnectionStatus Status;
}PooledSocks5Connection;

PooledSocks5Connection * socks5Pool;
time_t socksMaxTimeout = 100;

Socks5Connection * GetSocks5Connection();
void DestroySocks5Connection(Socks5Connection * connection);


Socks5Connection *CreateSocks5Connection(TcpConnection *tcpConnection) {
    LogInfo("Creating Socks5Connection.");

    Socks5Connection *connection = GetSocks5Connection();

    if (null == tcpConnection)
        LogError(false, "Cannot allocate space for Socks5Connection");

    connection->ClientTcpConnection = tcpConnection;
    connection->Handler = &socks5ConnectionHandler;
    connection->User = null;
    connection->Fsm.InitialState = CS_HELLO_READ;
    connection->Fsm.StatesSize = CS_ERROR;
    InitFsm(&connection->Fsm, socks5ConnectionFsm);

    void *readBuffer = calloc(1000, sizeof(byte));
    void *writeBuffer = calloc(1000, sizeof(byte));
    BufferInit(&connection->WriteBuffer, 1000, writeBuffer);
    BufferInit(&connection->ReadBuffer, 1000, readBuffer);

    RegisterConnectionInSocks5Metrics();

    LogInfo("Socks5Connection Created!");
    return connection;
}


void DisposeSocks5Connection(Socks5Connection *connection, fd_selector selector) {
    LogInfo("Disposing Socks5Connection...");
    if (null == connection) {
        LogError(false, "Cannot destroy NULL Socks5Connection");
        return;
    }

    RegisterDisconnectionInSocks5Metrics();

    if (null != connection->ClientTcpConnection)
        DisposeTcpConnection(connection->ClientTcpConnection, selector);

    if (null != connection->RemoteTcpConnection)
        DisposeTcpConnection(connection->RemoteTcpConnection, selector);

    if (null != connection->ReadBuffer.Data)
        free(connection->ReadBuffer.Data);

    if (null != connection->WriteBuffer.Data)
        free(connection->WriteBuffer.Data);

    if (null != connection->User)
        LogOutSocks5User(connection->User);

    DestroySocks5Connection(connection);
    LogInfo("Socks5Connection disposed!");
}



void CreateSocks5ConnectionPool(int initialSize) {
    LogInfo("Initializing SOCKS5 Pool");
    if (initialSize < 1) {
        LogInfo("Invalid initial pool size %d, using default value 1", initialSize);
        initialSize = 1;
    }

    socks5Pool = calloc(1, sizeof(PooledSocks5Connection));
    socks5Pool->Status = PooledSocks5ConnectionEmpty;
    PooledSocks5Connection *current = socks5Pool;

    while (--initialSize > 0) {
        current->Next = calloc(1, sizeof(PooledSocks5Connection));
        current = current->Next;
        current->Status = PooledSocks5ConnectionEmpty;
    }
}

void CleanSocks5ConnectionPool() {
    LogInfo("Clening SOCKS5 connection pool");
    if (null == socks5Pool) {
        LogError(false, "TCP pool was not initialized. Cannot clean it");
        return;
    }

    PooledSocks5Connection * next;
    for (PooledSocks5Connection * conn = socks5Pool; conn != null ; conn = next) {
        next = conn->Next;
        free(conn);
    }
}

void CheckForTimeoutInSocks5Connections(fd_selector fdSelector) {
    if (null == socks5Pool) {
        LogError(false, "SOCKS5 pool was not initialized");
    }
    if (socksMaxTimeout <= 0)
        return;

    time_t currentTime = time(null);
    if ((time_t) -1 == currentTime)
    {
        LogError(true,"Cannot get current time");
        return;
    }

    for (PooledSocks5Connection * temp = socks5Pool; temp != null ; temp = temp->Next){
        if (PooledSocks5ConnectionEmpty == temp->Status)
            continue;

        if (currentTime - temp->Connection.LastConnectionOn >= socksMaxTimeout) {
            LogInfo("Connection with client file descriptor %d and remote file descriptor %d timed out after %ld seconds",
                    temp->Connection.ClientTcpConnection->FileDescriptor,
                    temp->Connection.RemoteTcpConnection->FileDescriptor,
                    currentTime - temp->Connection.LastConnectionOn
                    );
            DisposeSocks5Connection(&temp->Connection, fdSelector);
        }
    }
}

void NotifySocks5ConnectionAccess(void *data) {
    Socks5Connection * connection = data;
    if (null == connection)
        return;

    connection->LastConnectionOn = time(null);

}

void SetConnectionTimeout(time_t timeout) {
    socksMaxTimeout = timeout;
}

Socks5Connection *GetSocks5Connection() {
    if (null == socks5Pool) {
        LogError(false, "SOCKS5 pool was not initialized");
        return null;
    }

    PooledSocks5Connection * temp;
    for ( temp = socks5Pool; temp != null ; temp = temp->Next) {
        if (PooledSocks5ConnectionEmpty == temp->Status) {
            temp->Status = PooledSocks5ConnectionInUse;
            return &temp->Connection;
        }

        if (null == temp->Next)
            break;
    }

    temp->Next = calloc(1, sizeof(PooledSocks5Connection));
    temp = temp->Next;
    temp->Status = PooledSocks5ConnectionInUse;

    return &temp->Connection;
}

void DestroySocks5Connection(Socks5Connection *connection) {
    if (null == socks5Pool) {
        LogError(false, "TCP pool was not initialized");
        return;
    }

    memset(connection,0, sizeof(Socks5Connection));

    PooledSocks5Connection * temp;
    for (temp = socks5Pool; temp != null && &temp->Connection != connection ; temp = temp->Next);

    if (null == temp)
    {
        LogError(false,"Error while destroying connection!");
        return;
    }
    assert(&temp->Connection == connection);
    temp->Status = PooledSocks5ConnectionEmpty;

}


#define ATTACHMENT(key) ( (Socks5Connection*)((SelectorKey*)(key))->Data)

void Socks5ConnectionRead(SelectorKey *key) {
    FiniteStateMachine *fsm = &ATTACHMENT(key)->Fsm;
    CONNECTION_STATE st = HandleReadFsm(fsm, key);

    if (CS_ERROR == st || CS_DONE == st) {
        DisposeSocks5Connection(ATTACHMENT(key), key->Selector);
    }
}

void Socks5ConnectionWrite(SelectorKey *key) {
    FiniteStateMachine *fsm = &ATTACHMENT(key)->Fsm;
    CONNECTION_STATE st = HandleWriteFsm(fsm, key);

    if (CS_ERROR == st || CS_DONE == st) {
        DisposeSocks5Connection(ATTACHMENT(key), key->Selector);
    }
}

void Socks5ConnectionBlock(SelectorKey *key) {
    FiniteStateMachine *fsm = &ATTACHMENT(key)->Fsm;
    CONNECTION_STATE st = HandleBlockFsm(fsm, key);

    if (CS_ERROR == st || CS_DONE == st) {
        DisposeSocks5Connection(ATTACHMENT(key), key->Selector);
    }
}

























