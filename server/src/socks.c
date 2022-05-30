//
// Created by Lucas Dell'Isola on 30/05/2022.
//

#include <malloc.h>
#include <string.h>
#include "../headers/socks.h"
#include "../headers/utils.h"
#include "../headers/logger.h"

#define SUPPORTED_SOCKS5_VERSION 0x05
#define SOCKS5_DEFAULT_USER "admin"
#define SOCKS5_DEFAULT_PASSWORD "admin"

typedef enum {
    SOCKS5_AUTH_NO_AUTH = 0,
    SOCKS5_AUTH_USER_PASS = 0x02,
    SOCKS5_AUTH_INVALID = 0xFF
}SOCKS5_AUTH;


int HandleInitHandshake(Socks5Connection * socks5Connection,byte * buffer, size_t length);
const char * GetSocks5AuthName(SOCKS5_AUTH socks5Auth);
int HandleAuthentication(Socks5Connection * socks5Connection,byte * buffer, size_t length);


TcpSocket *InitSocks5Server(const char *port) {
    LogInfo("Starting SOCKS5 server...");
    TcpSocket * tcpSocket = InitTcpServer(port);
    LogInfo("SOCKS5 server up and running!");
    return tcpSocket;
}

Socks5Connection * WaitForNewSocks5Connections(TcpSocket *server) {
    LogInfo( "Waiting for SOCKS5 connections...");

    TcpSocket * tcpSocket = WaitForNewConnections(server);
    if (tcpSocket == null) {
        LogError(false,"Could not establish SOCKS5 connection");
        return null;
    }

    LogInfo("SOCKS5 connection established");
    LogInfo("Starting SOCKS5 handshake...");

    Socks5Connection * socks5Connection = calloc(1, sizeof(Socks5Connection));
    socks5Connection->Socket = tcpSocket;
    socks5Connection->Status = SOCKS5_CS_INIT;

    return socks5Connection;
}

int HandleSocks5Request(Socks5Connection * socks5Connection,byte * buffer, size_t length) {
    if (socks5Connection == null) {
        LogError(false,"Invalid SOCKS5 connection. Cannot Handle it!");
        return ERROR;
    }

    LogInfo("Handling SOCKS5 request...");

    switch (socks5Connection->Status) {
        case SOCKS5_CS_INIT:
            return HandleInitHandshake(socks5Connection,buffer,length);
        case SOCKS5_CS_AUTH:
            return HandleAuthentication(socks5Connection,buffer,length);
        case SOCKS5_CA_READY:
            LogError(false,"Not Implemented");
            break;
    }
    return 0;
}

int DisposeSocks5Connection(Socks5Connection *socks5Connection) {
    if (socks5Connection == null)
    {
        LogError(false,"Cannot dispose null socks5 connection");
        return ERROR;
    }

    LogInfo("Disposing SOCKS5 connection...");
    DisposeTcpSocket(socks5Connection->Socket);
    free(socks5Connection);
    LogInfo("SOCKS5 connection disposed!");

    return OK;
}

int HandleAuthentication(Socks5Connection * socks5Connection,byte * buffer, size_t length){
    LogInfo("Handling authentication scheme...");
    // Invalid package size
    if (length < 5 || length > (1+1+1+255+255)) {
        LogError(false,"Invalid packet length: %d",length);
        return ERROR;
    }

    // Invalid SOCKS Protocol
    if (buffer[0] != SUPPORTED_SOCKS5_VERSION) {
        LogError(false,"Invalid SOCKS protocol version: %d. Expected: %d",buffer[0],SUPPORTED_SOCKS5_VERSION);
        return ERROR;
    }

    int userLength = buffer[1];
    // Invalid username length
    if(userLength < 1 || userLength > 255) {
        LogError(false,"Invalid username length: %d",userLength);
        return ERROR;
    }

    char username[255+1];
    strncpy(username, (const char *) (buffer + 2), userLength);

    LogInfo("Detected username: %s", username);

    int passLength = buffer[1 + 1 + userLength];
    // Invalid password length
    if (passLength < 1 || passLength > 255) {
        LogError(false,"Invalid password length %d",passLength);
        return ERROR;
    }

    char password[255+1];
    strncpy(password,(const char *) buffer+1+1+userLength+1, passLength);

    LogInfo("Detected password: %s", password);

    // TODO Implement real user system
    bool isLoggedIn =   strncmp(username,SOCKS5_DEFAULT_USER, strlen(SOCKS5_DEFAULT_USER)) == 0 &&
                        strncmp(password,SOCKS5_DEFAULT_PASSWORD,strlen(SOCKS5_DEFAULT_PASSWORD)) == 0;

    if (isLoggedIn){
        LogInfo("Log in successful!");
    }
    else
        LogInfo("Log in failed! Invalid username/password");

    byte data[2] = {SUPPORTED_SOCKS5_VERSION, isLoggedIn ? 0 : 0xFF};
    WriteToTcpSocket(socks5Connection->Socket,data,2);

    if (isLoggedIn)
        socks5Connection->Status = SOCKS5_CA_READY;
    else {
        LogInfo("Invalid credentials. Closing TCP socket");
        return DisconnectFromTcpSocket(socks5Connection->Socket);
    }


    return OK;
}


int HandleInitHandshake(Socks5Connection * socks5Connection,byte * buffer, size_t length){
    LogInfo( "Replying SOCKS5 handshake request...");
    // Max package size is 1 + 1 + 255
    if (length > 257 || length < 3) {
        LogError(false,"Invalid packet length: %d",length);
        return ERROR;
    }

    // Invalid Socks Protocol version
    if (buffer[0] != SUPPORTED_SOCKS5_VERSION) {
        LogError(false,"Invalid SOCKS protocol version: %d. Expected: %d",buffer[0],SUPPORTED_SOCKS5_VERSION);
        return ERROR;
    }
    LogInfo("SOCKS protocol version %d",buffer[0]);

    int methodsLength = buffer[1];
    // Invalid number of methods
    if (methodsLength < 1 || methodsLength > 255){
        LogError(false,"Invalid Authentication methods number: %d",methodsLength);
        return ERROR;
    }

    LogInfo("Detected %d methods to authenticate",methodsLength);

    SOCKS5_AUTH selectedAuthMethod = SOCKS5_AUTH_INVALID;

    for(int i = 0; i < methodsLength; i++)
    {
        if (buffer[2+i] == SOCKS5_AUTH_USER_PASS || buffer[2+i] == SOCKS5_AUTH_NO_AUTH)
        {
            selectedAuthMethod = buffer[2+1];
            break;
        }
    }

    LogInfo("Selected authentication method: %s", GetSocks5AuthName(selectedAuthMethod));

    byte data[2] = {SUPPORTED_SOCKS5_VERSION,selectedAuthMethod};
    if (WriteToTcpSocket(socks5Connection->Socket,data,2) < 0){
        return ERROR;
    }

    switch (selectedAuthMethod) {
        case SOCKS5_AUTH_NO_AUTH:
            socks5Connection->Status = SOCKS5_CA_READY;
            break;
        case SOCKS5_AUTH_USER_PASS:
            socks5Connection->Status = SOCKS5_CS_AUTH;
            break;
        default:
            DisconnectFromTcpSocket(socks5Connection->Socket);
            break;
    }

    return OK;
}

const char * GetSocks5AuthName(SOCKS5_AUTH socks5Auth) {
    switch (socks5Auth) {
        case SOCKS5_AUTH_NO_AUTH:
            return "No Authentication";
        case SOCKS5_AUTH_USER_PASS:
            return "Username/Password Authentication";
        default:
            return "Authentication Not Supported";
    }
}






















