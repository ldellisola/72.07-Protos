//
// Created by Lucas Dell'Isola on 06/06/2022.
//

#include <string.h>
#include "socks5/socks5_connection.h"
#include "socks5/socks5_hello.h"
#include "utils/logger.h"
#include "parsers/auth_parser.h"
#include "fsm/fsm.h"
#include "socks5/socks5_auth.h"
#include "socks5/socks5_request.h"
#include "socks5/socks5_establish_connection.h"
#include "socks5/socks5_connected.h"
#include "socks5/socks5_client.h"
#include "socks5/socks5_remote.h"
#include "socks5/socks5_dns.h"

// TODO: Test

static void Socks5ConnectionRead(SelectorKey *key);

static void Socks5ConnectionWrite(SelectorKey *key);

static void Socks5ConnectionBlock(SelectorKey *key);
//static void Socks5ConnectionClose(SelectorKey *key);

const FdHandler socks5ConnectionHandler = {
        .handle_read   = Socks5ConnectionRead,
        .handle_write  = Socks5ConnectionWrite,
//        .handle_close  = Socks5ConnectionClose,
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


Socks5Connection *Socks5ConnectionInit(TcpConnection *tcpConnection) {
    LogInfo("Creating Socks5Connection.");

    Socks5Connection *connection = calloc(1, sizeof(Socks5Connection));

    if (null == tcpConnection)
        LogError(false, "Cannot allocate space for Socks5Connection");

    connection->State = CS_HELLO_READ;
    connection->ClientTcpConnection = tcpConnection;
    connection->Handler = &socks5ConnectionHandler;
    connection->Fsm.InitialState = CS_HELLO_READ;
    connection->Fsm.StatesSize = CS_ERROR;
    InitFsm(&connection->Fsm, socks5ConnectionFsm);

    void *readBuffer = calloc(1000, sizeof(byte));
    void *writeBuffer = calloc(1000, sizeof(byte));
    BufferInit(&connection->WriteBuffer, 1000, writeBuffer);
    BufferInit(&connection->ReadBuffer, 1000, readBuffer);

    LogInfo("Socks5Connection Created!");
    return connection;
}


void Socks5ConnectionDestroy(Socks5Connection *connection, fd_selector selector) {
    LogInfo("Disposing Socks5Connection...");
    if (null == connection) {
        LogError(false, "Cannot destroy NULL Socks5Connection");
        return;
    }

    if (null != connection->ClientTcpConnection)
        DisposeTcpConnection(connection->ClientTcpConnection, selector);

    if (null != connection->RemoteTcpConnection)
        DisposeTcpConnection(connection->RemoteTcpConnection, selector);

    if (null != connection->ReadBuffer.Data)
        free(connection->ReadBuffer.Data);

    if (null != connection->WriteBuffer.Data)
        free(connection->WriteBuffer.Data);

    free(connection);
    LogInfo("Socks5Connection disposed!");
}


#define ATTACHMENT(key) ( (Socks5Connection*)((SelectorKey*)(key))->Data)

void Socks5ConnectionRead(SelectorKey *key) {
    FiniteStateMachine *fsm = &ATTACHMENT(key)->Fsm;
    CONNECTION_STATE st = HandleReadFsm(fsm, key);

    if (CS_ERROR == st || CS_DONE == st) {
        Socks5ConnectionDestroy(ATTACHMENT(key), key->Selector);
    }
}

void Socks5ConnectionWrite(SelectorKey *key) {
    FiniteStateMachine *fsm = &ATTACHMENT(key)->Fsm;
    CONNECTION_STATE st = HandleWriteFsm(fsm, key);

    if (CS_ERROR == st || CS_DONE == st) {
        Socks5ConnectionDestroy(ATTACHMENT(key), key->Selector);
    }
}

void Socks5ConnectionBlock(SelectorKey *key) {
    FiniteStateMachine *fsm = &ATTACHMENT(key)->Fsm;
    CONNECTION_STATE st = HandleBlockFsm(fsm, key);

    if (CS_ERROR == st || CS_DONE == st) {
        Socks5ConnectionDestroy(ATTACHMENT(key), key->Selector);
    }
}

//void Socks5ConnectionClose(SelectorKey *key) {
//    Socks5ConnectionDestroy(ATTACHMENT(key), key->Selector);
//}




















