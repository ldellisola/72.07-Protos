//
// Created by tluci on 19/6/2022.
//

#include "lulu/lulu_messages.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "utils/logger.h"
void fillBuffer(const char* string,byte * buffer, int length );

size_t BuildClientHelloResponse(byte *buffer, size_t length, bool authenticationSucceeded) {

    if(authenticationSucceeded){
        if (length < LEN_OK) {
            Error("Buffer to small to WriteHead ClientHelloResponse");
            return 0;
        }
        fillBuffer("+OK\r\n", buffer,LEN_OK );
        return LEN_OK;
    }else{
        if (length < LEN_AUTHENTICATION_ERROR) {
            Error("Buffer to small to WriteHead ClientHelloResponse");
            return 0;
        }
        fillBuffer("-AUTHENTICATION ERROR\r\n", buffer, LEN_AUTHENTICATION_ERROR);
        return LEN_AUTHENTICATION_ERROR;
    }
}

size_t BuildClientGoodbyeResponse(byte *buffer, size_t length) {

    if (length < LEN_GOODBYE) {
        Error("Buffer to small to WriteHead ClientHelloResponse");
        return 0;
    }
    fillBuffer("+GOODBYE\r\n", buffer,LEN_GOODBYE );

    return LEN_GOODBYE;
}
size_t BuildClientSetTimeoutResponse(byte *buffer, size_t length, size_t timeout) {

//    TODO: DONDE ESTA timeout

    if (length < LEN_OK) {
        Error("Buffer to small to WriteHead ClientSetTimeoutResponse");
        return 0;
    }
    fillBuffer("+OK\r\n", buffer,LEN_OK );

    return LEN_OK;
}
size_t BuildClientSetBufferSizeResponse(byte *buffer, size_t length,int64_t bufferSize ) {
    bool validBufferSize;
    if(bufferSize <= 0 || bufferSize >= 10000000000){
        validBufferSize = false;
    }

    if(validBufferSize){
        SetSocks5BufferSize(bufferSize);
        if (length < LEN_OK) {
            Error("Buffer to small to WriteHead ClientSetBufferSizeResponse");
            return 0;
        }
        fillBuffer("+OK\r\n", buffer,LEN_OK );
        return LEN_OK;
    }else{
        if (length < LEN_INVALID_BUFFER_SIZE) {
            Error("Buffer to small to WriteHead ClientSetBufferSizeResponse");
            return 0;
        }
        fillBuffer("-INVALID BUFFER SIZE\r\n", buffer,LEN_INVALID_BUFFER_SIZE );
        return LEN_INVALID_BUFFER_SIZE;
    }
}
size_t BuildClientGetTimeoutResponse(byte *buffer, size_t length) {
    int timeout = 0;
//    TODO: DONDE ESTA TIMEOUT
    uint64_t nDigits = 1;
    if(timeout !=0){
        nDigits = floor(log10(abs(timeout))) + 1;
    }

    if (length < (nDigits + 3)) {
        Error("Buffer to small to WriteHead ClientGetTimeoutResponse");
        return 0;
    }
    char str[nDigits+3];
    sprintf(str, "+%d\r", timeout);
    str[nDigits+2] = '\n';
    fillBuffer(str, buffer,(int)nDigits+3 );

    return nDigits+3;
}
size_t BuildClientGetBufferSizeResponse(byte *buffer, size_t length) {
    size_t bufferSize;
    bufferSize = GetSocks5BufferSize();
    uint64_t nDigits = 1;
    if(bufferSize !=0){
        nDigits = floor(log10(bufferSize)) + 1;
    }

    if (length < (nDigits + 3)) {
        Error("Buffer to small to WriteHead ClientGetBufferSizeResponse");
        return 0;
    }
    char str[nDigits+3];
    sprintf(str, "+%lu\r", bufferSize);
    str[nDigits+2] = '\n';
    fillBuffer(str, buffer,(int)nDigits+3 );

    return nDigits+3;
}

size_t BuildClientGetMetricsResponse(byte *buffer, size_t length) {
    Socks5Metrics *metrics = GetSocks5Metrics();
    size_t nDigits = 0;
    nDigits += metrics->BytesTransferred !=0? (floor(log10(metrics->BytesTransferred)) + 1) : 1;
    nDigits += metrics->CurrentConnections !=0? (floor(log10(metrics->CurrentConnections)) + 1) : 1;
    nDigits += metrics->HistoricalConnections !=0? (floor(log10(metrics->HistoricalConnections)) + 1) : 1;

    if (length < (nDigits + 5)) {
        Error("Buffer to small to WriteHead ClientGetBufferSizeResponse");
        return 0;
    }
    char str[nDigits+5];
    sprintf(str, "+%lu|%lu|%lu\r", metrics->HistoricalConnections, metrics->CurrentConnections, metrics->BytesTransferred);
    str[nDigits+4] = '\n';
    fillBuffer(str, buffer,(int)nDigits+5 );

    return nDigits+3;
}

size_t BuildClientListUsersResponse(byte *buffer, size_t length) {
//    TODO:freee
    char **usernames = calloc(200, sizeof(byte));
    int users = GetAllLoggedInSocks5Users(usernames,200 );
    char str[] ="+";
    users = (users == -1)? 200:users;
    for (int i = 0; i < users; ++i) {
        strcat(str,usernames[i]);
        strcat(str,(i==users-1)? "\r\n":"|");
    }
    if (length < (strlen(str)-1) ) {
        Error("Buffer to small to WriteHead ClientSetUserSizeResponse");
        return 0;
    }
    fillBuffer(str, buffer,(int) (strlen(str)-1) );

    return (strlen(str)-1);
}

size_t BuildClientSetUserResponse(byte *buffer, size_t length, char* username, char* password) {

//    todo: free?
    char **loggedUsernames = calloc(200, sizeof(byte));
    int users = GetAllLoggedInSocks5Users(loggedUsernames,200 );

    users = (users == -1)? 200:users;

    for (int i = 0; i < users; ++i) {
        if(strcmp(loggedUsernames[i], username) == 0){
            if (length < LEN_USER_EXISTS) {
                Error("Buffer to small to WriteHead ClientSetUserSizeResponse");
                return 0;
            }
            fillBuffer("-USER EXISTS\r\n", buffer,LEN_USER_EXISTS );
            return 14;
        }
    }
//    TODO: free?
    const char** usernames = calloc(2, sizeof(byte));
    const char** passwords= calloc(2, sizeof(byte));;
    usernames[0] = username;
    usernames[1] = null;
    passwords[0] = password;
    passwords[1] = null;
    LoadSocks5Users(usernames,passwords );

    if (length < LEN_OK) {
        Error("Buffer to small to WriteHead ClientSetUserSizeResponse");
        return 0;
    }
    fillBuffer("+OK\r\n", buffer,LEN_OK );

    return (LEN_OK);
}

size_t BuildClientDelUserResponse(byte *buffer, size_t length, char* username) {
    int status = DeleteSocks5User(username);
    switch (status) {
        case OK:
            if (length < LEN_OK) {
                Error("Buffer to small to WriteHead ClientDelUserSizeResponse");
                return 0;
            }
            fillBuffer("+OK\r\n", buffer,LEN_OK );

            return (LEN_OK);
        case LOGGED_IN:
            if (length < LEN_USER_LOGGED_IN) {
                Error("Buffer to small to WriteHead ClientDelUserSizeResponse");
                return 0;
            }
            fillBuffer("-USER LOGGED IN\r\n", buffer,LEN_USER_LOGGED_IN );

            return (LEN_USER_LOGGED_IN);
        case DOESNT_EXIST:
            if (length < LEN_USER_DOESNT_EXIST) {
                Error("Buffer to small to WriteHead ClientDelUserSizeResponse");
                return 0;
            }
            fillBuffer("-USER DOESNT EXIST\r\n", buffer,LEN_USER_DOESNT_EXIST );

            return (DOESNT_EXIST);
        default:
            break;

    }
    return -1;
}

size_t BuildClientNotRecognisedResponse(byte *buffer, size_t length) {

    if (length < LEN_NOT_RECOGNISED) {
        Error("Buffer to small to WriteHead ClientNotRecognisedResponse");
        return 0;
    }
    fillBuffer("-NOT RECOGNISED\r\n", buffer,LEN_NOT_RECOGNISED );

    return LEN_NOT_RECOGNISED;
}
void fillBuffer(const char* string,byte * buffer, int length ){
    for (int i = 0; i < length; i++) {
        buffer[i] = string[i];
    }
}
