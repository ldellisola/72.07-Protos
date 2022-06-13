//
// Created by Lucas Dell'Isola on 06/06/2022.
//

#include <string.h>
#include "socks5/socks5_connection.h"
#include "socks5/socks5_hello.h"
#include "utils/logger.h"
#include "parsers/hello_parser.h"
#include "parsers/auth_parser.h"
#include "parsers/request_parser.h"
#include "socks5/socks5_messages.h"
#include "fsm/fsm.h"
#include "socks5/socks5_auth.h"

// TODO: Test





static void Socks5ConnectionRead(SelectorKey *key);
static void Socks5ConnectionWrite(SelectorKey *key);
static void Socks5ConnectionBlock(SelectorKey *key);
static void Socks5ConnectionClose(SelectorKey *key);

FdHandler socks5ConnectionHandler = {
        .handle_read   = Socks5ConnectionRead,
        .handle_write  = Socks5ConnectionWrite,
        .handle_close  = Socks5ConnectionClose,
        .handle_block  = Socks5ConnectionBlock,
};

void nothing(unsigned aa,void * sss){

}

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
            .state = CS_DONE,
                .on_arrival = nothing
        },
        {
                .state = CS_ERROR,
                .on_arrival = nothing
        },
};


bool HandleHello(Socks5Connection * connection, byte* data, int length);
bool HandleAuthentication(Socks5Connection *connection, byte *data, int length);
bool HandleRequest(Socks5Connection *connection, byte *data, int length);



Socks5Connection *Socks5ConnectionInit(TcpConnection *tcpConnection) {
    LogInfo("Creating Socks5Connection.");

    Socks5Connection *  connection = calloc(1, sizeof(Socks5Connection));

    if (null == tcpConnection)
        LogError(false,"Cannot allocate space for Socks5Connection");

    connection->State = CS_HELLO_READ;
    connection->ClientTcpConnection = tcpConnection;
    connection->Handler = &socks5ConnectionHandler;
    connection->Fsm.InitialState = CS_HELLO_READ;
    connection->Fsm.StatesSize = CS_ERROR;
    InitFsm(&connection->Fsm,socks5ConnectionFsm);

    void * readBuffer = calloc(500,sizeof(byte));
    void * writeBuffer = calloc(500,sizeof(byte));
    BufferInit(&connection->WriteBuffer,500,writeBuffer);
    BufferInit(&connection->ReadBuffer,500,readBuffer);

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



//bool HandleRequest(Socks5Connection *connection, byte *data, int length) {
//    RequestParser * parser = connection->Parser.Request;
//   // TODO: Figure out return value
//   RequestParserConsume(parser,data,length);
//
//   if (!RequestParserHasFinished(parser->State))
//       return false;
//
//   bool hasFailed = RequestParserHasFinished(parser->State);
//   if (hasFailed){
//       // TODO: Handle error
//       connection->State = SOCKS5_CS_FAILED;
//       return false;
//   }
//
//   byte message[1024];
//   int messageSize;
//   if (parser->CMD != SOCKS5_CMD_CONNECT){
//       // TODO: Send method not implemented
//       messageSize = BuildRequestResponseFromParser(message,1024,SOCKS5_REPLY_COMMAND_NOT_SUPPORTED,parser);
//       WriteToTcpConnection(connection->ClientTcpConnection, message, messageSize);
//       return false;
//   }
//
//   if (parser->AType != SOCKS5_ADDRESS_TYPE_IPV4){
//       // TODO: support other address type
//       // TODO: Send Method not implemented
//       messageSize = BuildRequestResponseFromParser(message,1024,SOCKS5_REPLY_ADDRESS_TYPE_NOT_SUPPORTED,parser);
//       WriteToTcpConnection(connection->ClientTcpConnection, message, messageSize);
//       return false;
//   }
//
//   ClientTcpConnection * other = ConnectToTcpServer(parser->DestAddress, parser->DestPort);
//   if (null == other){
//       messageSize = BuildRequestResponseFromParser(message,1024,SOCKS5_REPLY_GENERAL_FAILURE,parser);
//       WriteToTcpConnection(connection->ClientTcpConnection, message, messageSize);
//       return false;
//   }
//
//
//
//
//   // TODO: Reply
//
//
//    return true;
//
//}
//
//
//bool HandleAuthentication(Socks5Connection *connection, byte *data, int length) {
//    AuthParser *parser = connection->Parser.Auth;
//    // TODO: Figure out return value
//    AuthParserConsume(parser, data, length);
//
//    if (!AuthParserHasFinished(parser->State))
//        return false;
//
//    if (AuthParserHasFailed(parser->State)){
//        connection->State = SOCKS5_CS_FAILED;
//        return true;
//    }
//
//    // TODO Implement real user system
//    bool isLoggedIn =   strncmp(parser->UName,SOCKS5_DEFAULT_USER, strlen(SOCKS5_DEFAULT_USER)) == 0 &&
//                        strncmp(parser->Passwd,SOCKS5_DEFAULT_PASSWORD,strlen(SOCKS5_DEFAULT_PASSWORD)) == 0;
//
//
//    // TODO: Make non blocking
//    byte message[2];
//    int messageLength = BuildAuthResponse(message,2,isLoggedIn);
//    WriteToTcpConnection(connection->ClientTcpConnection, message, messageLength);
//
//    connection->State = isLoggedIn ? SOCKS5_CS_REQUEST : SOCKS5_CS_FINISHED;
//
//    return !isLoggedIn;
//}

#define ATTACHMENT(key) ( (Socks5Connection*)((SelectorKey*)(key))->Data)

void Socks5ConnectionDone(SelectorKey * key);

void Socks5ConnectionRead(SelectorKey *key) {
    FiniteStateMachine *fsm   = &ATTACHMENT(key)->Fsm;
    CONNECTION_STATE st = HandleReadFsm(fsm, key);

    if(CS_ERROR == st || CS_DONE == st) {
        Socks5ConnectionDestroy(ATTACHMENT(key), key->Selector);
    }
}

void Socks5ConnectionWrite(SelectorKey *key) {
    FiniteStateMachine *fsm   = &ATTACHMENT(key)->Fsm;
    CONNECTION_STATE st = HandleWriteFsm(fsm, key);

    if(CS_ERROR == st || CS_DONE == st) {
        Socks5ConnectionDestroy(ATTACHMENT(key), key->Selector);
    }
}

void Socks5ConnectionBlock(SelectorKey *key) {
    FiniteStateMachine *fsm   = &ATTACHMENT(key)->Fsm;
    CONNECTION_STATE st = HandleBlockFsm(fsm, key);

    if(CS_ERROR == st || CS_DONE == st) {
        Socks5ConnectionDestroy(ATTACHMENT(key), key->Selector);
    }
}

void Socks5ConnectionClose(SelectorKey *key) {
    Socks5ConnectionDestroy(ATTACHMENT(key), key->Selector);
}




















