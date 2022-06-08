//
// Created by Lucas Dell'Isola on 06/06/2022.
//

#include <string.h>
#include "socks5/socks5_connection.h"
#include "utils/logger.h"
#include "parsers/hello_parser.h"
#include "parsers/auth_parser.h"
#include "parsers/request_parser.h"
#include "socks5/socks5_messages.h"

// TODO: Test

typedef enum {
    SOCKS5_AUTH_NO_AUTH = 0,
    SOCKS5_AUTH_USER_PASS = 0x02,
    SOCKS5_AUTH_INVALID = 0xFF
}SOCKS5_AUTH;


bool HandleHello(Socks5Connection * connection, byte* data, int length);


bool HandleAuthentication(Socks5Connection *connection, byte *data, int length);

bool HandleRequest(Socks5Connection *connection, byte *data, int length);

#define SOCKS5_DEFAULT_USER "admin"
#define SOCKS5_DEFAULT_PASSWORD "admin"

Socks5Connection *Socks5ConnectionInit(TcpSocket * tcpSocket) {
    LogInfo("Creating Socks5Connection.");
    if (null == tcpSocket){
        LogError(false,"TcpSocket cannot be NULL");
        return null;
    }
    Socks5Connection *  connection = calloc(1, sizeof(Socks5Connection));

    if (null == connection)
        LogError(false,"Cannot allocate space for Socks5Connection");

    connection->Status = SOCKS5_CS_INIT;
    connection->Socket = tcpSocket;
    connection->Parser = HelloParserInit();
    connection->ParserType = HelloParserType;

    LogInfo("Socks5Connection Created!");
    return connection;
}


void Socks5ConnectionDestroy(Socks5Connection *connection) {
    LogInfo("Disposing Socks5Connection...");
    if (null == connection) {
        LogError(false, "Cannot destroy NULL Socks5Connection");
        return;
    }

    if (null != connection->Socket)
        DisposeTcpSocket(connection->Socket);

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

    switch (connection->Status) {
        case SOCKS5_CS_INIT: return HandleHello(connection, data, length);
        case SOCKS5_CS_AUTH: return HandleAuthentication(connection, data, length);
        case SOCKS5_CS_REQUEST: return HandleRequest(connection,data,length);
        case SOCKS5_CS_READY:
            break;
        case SOCKS5_CS_FINISHED:
            return true;
        case SOCKS5_CS_FAILED:
            break;
    }

    return false;

}

bool Socks5ConnectionFailed(Socks5Connection *connection) {
    if (null == connection)
    {
        LogError(false,"Socks5Connection cannot be NULL to check if it failed");
        return true;
    }

    if (SOCKS5_CS_FAILED == connection->Status)
        return true;

    return false;
}

/*
 * +----+-----+-------+------+----------+----------+
        |VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
        +----+-----+-------+------+----------+----------+
        | 1  |  1  | X’00’ |  1   | Variable |    2     |
        +----+-----+-------+------+----------+----------+
Where:
          o  VER    protocol version: X’05’
          o  REP    Reply field:
          o  RSV    RESERVED
          o  ATYP   address type of following address
             o  IP V4 address: X’01’
             o  DOMAINNAME: X’03’
             o  IP V6 address: X’04’
          o  BND.ADDR       server bound address
          o  BND.PORT       server bound port in network octet order

 */


bool HandleRequest(Socks5Connection *connection, byte *data, int length) {
    RequestParser * parser = connection->Parser;
   // TODO: Figure out return value
   RequestParserConsume(parser,data,length);

   if (!RequestParserHasFinished(parser->State))
       return false;

   bool hasFailed = RequestParserHasFinished(parser->State);
   if (hasFailed){
       // TODO: Handle error
       connection->Status = SOCKS5_CS_FAILED;
       return false;
   }

   if (parser->CMD != CMD_CONNECT){
       // TODO: Send method not implemented
       return false;
   }

   if (SOCKS5_ADDRESS_TYPE_FQDN == parser->AType){
       // TODO: Handle DNS
       return false;
   }
   // TODO: Connect to other socket

   // TODO: Reply
//   byte message[] = {
//           SOCKS5_PROTOCOL_VERSION,
//           SocksRequest_Succeeded,
//           0,
//           SOCKS5_ADDRESS_TYPE_IPV4,
//
//
//   };

    return true;

}

bool HandleHello(Socks5Connection * connection, byte* data, int length) {
    HelloParser * parser = connection->Parser;
    // TODO: Figure out return value
    HelloParserConsume(parser, data, length);
    if (!HelloParserHasFinished(parser->State))
        return false;

    bool hasFailed = HelloParserHasFailed(parser->State);
    // TODO: Handle Error
    if (hasFailed) {
        connection->Status = SOCKS5_CS_FAILED;
        return false;
    }

    SOCKS5_AUTH selectedAuthMethod = SOCKS5_AUTH_INVALID;

    for(int i = 0; i < parser->NMethods; i++)
    {
        if ( SOCKS5_AUTH_USER_PASS == parser->Methods[i] || SOCKS5_AUTH_NO_AUTH == parser->Methods[i])
        {
            selectedAuthMethod = parser->Methods[i];
            break;
        }
    }

    byte message[2];
    int messageLength = BuildHelloResponse(message,2,selectedAuthMethod);
    // TODO: Make non blocking
    WriteToTcpSocket(connection->Socket,message,messageLength);

    // TODO: Get IP if FQDN
    HelloParserDestroy(parser);

    switch (selectedAuthMethod) {
        case SOCKS5_AUTH_NO_AUTH:
            connection->Parser = RequestParserInit();
            connection->ParserType = RequestParserType;
            connection->Status = SOCKS5_CS_REQUEST;
            break;
        case SOCKS5_AUTH_USER_PASS:
            connection->Parser = AuthParserInit();
            connection->ParserType = AuthParserType;
            connection->Status = SOCKS5_CS_AUTH;
            break;
        default:
        case SOCKS5_AUTH_INVALID:
            connection->Status = SOCKS5_CS_FAILED;
            return true;
    }

    return false;
}

bool HandleAuthentication(Socks5Connection *connection, byte *data, int length) {
    AuthParser *parser = connection->Parser;
    // TODO: Figure out return value
    AuthParserConsume(parser, data, length);

    if (!AuthParserHasFinished(parser->State))
        return false;

    if (AuthParserHasFailed(parser->State)){
        connection->Status = SOCKS5_CS_FAILED;
        return true;
    }

    // TODO Implement real user system
    bool isLoggedIn =   strncmp(parser->UName,SOCKS5_DEFAULT_USER, strlen(SOCKS5_DEFAULT_USER)) == 0 &&
                        strncmp(parser->Passwd,SOCKS5_DEFAULT_PASSWORD,strlen(SOCKS5_DEFAULT_PASSWORD)) == 0;


    // TODO: Make non blocking
    byte message[2];
    int messageLength = BuildAuthResponse(message,2,isLoggedIn);
    WriteToTcpSocket(connection->Socket, message, messageLength);

    connection->Status = isLoggedIn ? SOCKS5_CS_REQUEST : SOCKS5_CS_FINISHED;

    return !isLoggedIn;
}


















