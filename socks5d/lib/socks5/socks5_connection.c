//
// Created by Lucas Dell'Isola on 06/06/2022.
//
#include "socks5/socks5_connection.h"
#include <string.h>
#include <assert.h>
#include <time.h>
#include "socks5/fsm_handlers/socks5_hello.h"
#include "utils/logger.h"
#include "parsers/socks5/auth_parser.h"
#include "fsm/fsm.h"
#include "socks5/fsm_handlers/socks5_auth.h"
#include "socks5/fsm_handlers/socks5_request.h"
#include "socks5/fsm_handlers/socks5_establish_connection.h"
#include "socks5/fsm_handlers/socks5_connected.h"
#include "socks5/fsm_handlers/socks5_client.h"
#include "socks5/fsm_handlers/socks5_remote.h"
#include "socks5/fsm_handlers/socks5_dns.h"
#include "socks5/socks5_metrics.h"
#include "socks5/socks5_buffer.h"
#include "utils/object_pool.h"

static void Socks5ConnectionRead(SelectorKey *key);
static void Socks5ConnectionWrite(SelectorKey *key);
static void Socks5ConnectionBlock(SelectorKey *key);

static const FdHandler socks5ConnectionHandler = {
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
                .on_arrival = ConnectedConnectionInit,
                .on_read_ready = ConnectedConnectionRun,
                .on_departure = ConnectedConnectionClose
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

void delete(void * obj){
    memset(obj,0, sizeof(Socks5Connection));
}

static ObjectPoolHandlers poolHandlers = {
        .OnDispose = delete,
        .OnCreate = null
};

static ObjectPool socks5Pool;
static time_t socksMaxTimeout = 100;



Socks5Connection *CreateSocks5Connection(TcpConnection *tcpConnection) {
    Debug("Creating Socks5Connection.");

    Socks5Connection *connection = GetObjectFromPool(&socks5Pool);

    if (null == tcpConnection)
        Error("Cannot allocate space for Socks5Connection");

    connection->ClientTcpConnection = tcpConnection;
    connection->Handler = &socks5ConnectionHandler;
    connection->User = null;
    connection->RemoteAddressString = null;
    connection->Fsm.InitialState = CS_HELLO_READ;
    connection->Fsm.StatesSize = CS_ERROR;
    InitFsm(&connection->Fsm, socks5ConnectionFsm);

    // TODO: Confirmar con lu
    BufferInit(&connection->WriteBuffer,10, calloc(10, sizeof(byte)));
    BufferInit(&connection->ReadBuffer,10, calloc(10, sizeof(byte)));
//    InitSocks5Buffer(&connection->WriteBuffer);
//    InitSocks5Buffer(&connection->ReadBuffer);

    RegisterConnectionInSocks5Metrics();

    Debug("Socks5Connection Created!");
    return connection;
}


void DisposeSocks5Connection(Socks5Connection *connection, fd_selector selector) {
    Debug("Disposing Socks5Connection...");
    if (null == connection) {
        Error( "Cannot destroy NULL Socks5Connection");
        return;
    }

    RegisterDisconnectionInSocks5Metrics();

    if (null != connection->ClientTcpConnection)
        DisposeTcpConnection(connection->ClientTcpConnection, selector);

    if (null != connection->RemoteTcpConnection)
        DisposeTcpConnection(connection->RemoteTcpConnection, selector);

    DisposeSocks5Buffer(&connection->WriteBuffer);
    DisposeSocks5Buffer(&connection->ReadBuffer);

    if (null != connection->User)
        LogOutSocks5User(connection->User);

    if (null != connection->RemoteAddressString)
        free(connection->RemoteAddressString);

    DestroyObject(&socks5Pool, connection);
    Debug("Socks5Connection disposed!");
}



void CreateSocks5ConnectionPool(int initialSize) {
    Debug("Initializing SOCKS5 Pool");
    InitObjectPool(&socks5Pool, &poolHandlers, initialSize, sizeof(Socks5Connection));
}

void CleanSocks5ConnectionPool() {
    Debug("Cleaning SOCKS5 connection socks5Pool");
    CleanObjectPool(&socks5Pool);
}

void CheckForTimeoutInSingleConnection(void * obj, void * data){
    Socks5Connection * connection = obj;
    fd_selector fdSelector = data;

    if (socksMaxTimeout <= 0)
        return;

    time_t currentTime = time(null);
    if ((time_t) -1 == currentTime)
    {
        ErrorWithReason("Cannot get current time");
        return;
    }

    if (currentTime - connection->LastConnectionOn >= socksMaxTimeout) {
        if (null == connection->RemoteAddressString) {
            LogInfo("Connection with client file descriptor %d and remote file descriptor %d timed out after %ld seconds",
                    connection->ClientTcpConnection->FileDescriptor,
                    connection->RemoteTcpConnection->FileDescriptor,
                    currentTime - connection->LastConnectionOn
            );
        }
        else{
            LogInfo("Connection to %s:%d timed out after %d seconds",
                    connection->RemoteAddressString,
                    connection->RemotePort,
                    currentTime - connection->LastConnectionOn
            );
        }
        DisposeSocks5Connection(connection, fdSelector);
    }


}

void CheckForTimeoutInSocks5Connections(fd_selector fdSelector) {

    ExecuteOnExistingElements(&socks5Pool, CheckForTimeoutInSingleConnection, fdSelector);

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

























