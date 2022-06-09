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

//static const StateDefinition socks5ConnectionFsm[] = {
//        {
//            .state = CS_HELLO_READ,
//            .on_arrival = HelloReadInit,
//            .on_departure = HelloReadClose,
//            .on_read_ready = HelloReadRun
//        },
//        {
//            .state = CS_HELLO_WRITE,
//        },
//        {
//            .state = CS_ERROR
//        },
//        {
//            .state = CS_DONE
//        }
//};


bool HandleHello(Socks5Connection * connection, byte* data, int length);
bool HandleAuthentication(Socks5Connection *connection, byte *data, int length);
bool HandleRequest(Socks5Connection *connection, byte *data, int length);

#define SOCKS5_DEFAULT_USER "admin"
#define SOCKS5_DEFAULT_PASSWORD "admin"

Socks5Connection *Socks5ConnectionInit(TcpConnection *tcpConnection) {
    LogInfo("Creating Socks5Connection.");

    Socks5Connection *  connection = calloc(1, sizeof(Socks5Connection));

    if (null == tcpConnection)
        LogError(false,"Cannot allocate space for Socks5Connection");

//    connection->State = SOCKS5_CS_INIT;
//    connection->Parser.Hello = HelloParserInit();
    connection->TcpConnection = tcpConnection;
    connection->Handler = &socks5ConnectionHandler;

    void * readBuffer = calloc(500,sizeof(byte));
    void * writeBuffer = calloc(500,sizeof(byte));
    BufferInit(&connection->WriteBuffer,500,writeBuffer);
    BufferInit(&connection->ReadBuffer,500,readBuffer);

    LogInfo("Socks5Connection Created!");
    return connection;
}


void Socks5ConnectionDestroy(Socks5Connection *connection) {
    LogInfo("Disposing Socks5Connection...");
    if (null == connection) {
        LogError(false, "Cannot destroy NULL Socks5Connection");
        return;
    }

    if (null != connection->TcpConnection)
        DisposeTcpConnection(connection->TcpConnection);

    if (null != connection->ReadBuffer.Data)
        free(connection->ReadBuffer.Data);

    if (null != connection->WriteBuffer.Data)
        free(connection->WriteBuffer.Data);

    free(connection);
    LogInfo("Socks5Connection disposed!");
}



bool RunSocks5(Socks5Connection *connection, byte *data, int length) {
    LogInfo("Running Socks5Connection FSM");

    if(null == connection)
    {
        LogError(false,"Socks5Connection cannot be NULL");
        return true;
    }

//    switch (connection->State) {
//        case SOCKS5_CS_INIT: return HandleHello(connection, data, length);
//        case SOCKS5_CS_AUTH: return HandleAuthentication(connection, data, length);
//        case SOCKS5_CS_REQUEST: return HandleRequest(connection,data,length);
//        case SOCKS5_CS_READY:
//            break;
//        case SOCKS5_CS_FINISHED:
//            return true;
//        case SOCKS5_CS_FAILED:
//            break;
//    }

    return false;

}

bool Socks5ConnectionFailed(Socks5Connection *connection) {
    if (null == connection)
    {
        LogError(false,"Socks5Connection cannot be NULL to check if it failed");
        return true;
    }

//    if (SOCKS5_CS_FAILED == connection->State)
//        return true;

    return false;
}

//
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
//       WriteToTcpConnection(connection->TcpConnection, message, messageSize);
//       return false;
//   }
//
//   if (parser->AType != SOCKS5_ADDRESS_TYPE_IPV4){
//       // TODO: support other address type
//       // TODO: Send Method not implemented
//       messageSize = BuildRequestResponseFromParser(message,1024,SOCKS5_REPLY_ADDRESS_TYPE_NOT_SUPPORTED,parser);
//       WriteToTcpConnection(connection->TcpConnection, message, messageSize);
//       return false;
//   }
//
//   TcpConnection * other = ConnectToTcpServer(parser->DestAddress, parser->DestPort);
//   if (null == other){
//       messageSize = BuildRequestResponseFromParser(message,1024,SOCKS5_REPLY_GENERAL_FAILURE,parser);
//       WriteToTcpConnection(connection->TcpConnection, message, messageSize);
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
//    WriteToTcpConnection(connection->TcpConnection, message, messageLength);
//
//    connection->State = isLoggedIn ? SOCKS5_CS_REQUEST : SOCKS5_CS_FINISHED;
//
//    return !isLoggedIn;
//}

void Socks5ConnectionRead(SelectorKey *key) {

}

void Socks5ConnectionWrite(SelectorKey *key) {

}

void Socks5ConnectionBlock(SelectorKey *key) {

}

void Socks5ConnectionClose(SelectorKey *key) {

}




















