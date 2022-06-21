//
// Created by tluci on 19/6/2022.
//

#include "lulu/lulu_messages.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "utils/logger.h"
#include "socks5/socks5_timeout.h"
void fillBuffer(const char* string,byte * buffer, int length );

size_t BuildClientHelloResponse(byte *buffer, size_t length, bool authenticationSucceeded) {

    if(authenticationSucceeded){
        if (length < LEN_OK) {
            Warning("Buffer to small to WriteHead ClientHelloResponse");
            return 0;
        }
        fillBuffer("+OK\r\n", buffer,LEN_OK );
        return LEN_OK;
    }else{
        if (length < LEN_AUTHENTICATION_ERROR) {
            Warning("Buffer to small to WriteHead ClientHelloResponse");
            return 0;
        }
        fillBuffer("-AUTHENTICATION ERROR\r\n", buffer, LEN_AUTHENTICATION_ERROR);
        return LEN_AUTHENTICATION_ERROR;
    }
}

size_t BuildClientGoodbyeResponse(byte *buffer, size_t length) {

    if (length < LEN_GOODBYE) {
        Warning("Buffer to small to WriteHead ClientHelloResponse");
        return 0;
    }
    fillBuffer("+GOODBYE\r\n", buffer,LEN_GOODBYE );

    return LEN_GOODBYE;
}
size_t BuildClientSetTimeoutResponse(byte *buffer, size_t length, size_t timeout) {

    SetSocks5ConnectionTimeout(timeout);
    if (length < LEN_OK) {
        Warning("Buffer to small to WriteHead ClientSetTimeoutResponse");
        return 0;
    }
    fillBuffer("+OK\r\n", buffer,LEN_OK );

    return LEN_OK;
}
size_t BuildClientSetBufferSizeResponse(byte *buffer, size_t length,size_t bufferSize ) {
    bool validBufferSize = true;
    if(bufferSize <= 0 || bufferSize >= 10000000000){
        validBufferSize = false;
    }

    if(validBufferSize){
        SetSocks5BufferSize(bufferSize);
        if (length < LEN_OK) {
            Warning("Buffer to small to WriteHead ClientSetBufferSizeResponse");
            return 0;
        }
        fillBuffer("+OK\r\n", buffer,LEN_OK );
        return LEN_OK;
    }else{
        if (length < LEN_INVALID_BUFFER_SIZE) {
            Warning("Buffer to small to WriteHead ClientSetBufferSizeResponse");
            return 0;
        }
        fillBuffer("-INVALID BUFFER SIZE\r\n", buffer,LEN_INVALID_BUFFER_SIZE );
        return LEN_INVALID_BUFFER_SIZE;
    }
}
size_t BuildClientGetTimeoutResponse(byte *buffer, size_t length) {
    long timeout = 0;
    timeout = GetSocks5ConnectionTimeout();
    char num[15] = {0};
    sprintf(num, "+%ld\r\n", timeout);
    size_t nDigits = strlen(num);


    if (length < (nDigits)) {
        Warning("Buffer to small to WriteHead ClientGetTimeoutResponse");
        return 0;
    }

    fillBuffer(num, buffer,(int)nDigits );

    return nDigits;
}
size_t BuildClientGetBufferSizeResponse(byte *buffer, size_t length) {
    size_t bufferSize;
    bufferSize = GetSocks5BufferSize();

    char num[11]={0};
    sprintf(num, "%lu", bufferSize);
    size_t nDigits= strlen(num);

    if (length < (nDigits + 3)) {
        Warning("Buffer to small to WriteHead ClientGetBufferSizeResponse");
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
    char num[11]={0};
    sprintf(num, "%lu", metrics->BytesTransferred);
    nDigits+= strlen(num);
    sprintf(num, "%lu", metrics->CurrentConnections);
    nDigits+= strlen(num);
    sprintf(num, "%lu", metrics->HistoricalConnections);
    nDigits+= strlen(num);
    nDigits+=5;
    if (length < (nDigits)) {
        Warning("Buffer to small to WriteHead ClientGetBufferSizeResponse");
        return 0;
    }
    char str[nDigits];
    sprintf(str, "+%lu|%lu|%lu\r", metrics->HistoricalConnections, metrics->CurrentConnections, metrics->BytesTransferred);
    str[nDigits-1] = '\n';
    fillBuffer(str, buffer,(int)nDigits );

    return nDigits;
}

size_t BuildClientListUsersResponse(byte *buffer, size_t length) {
//    TODO:freee / listo
    char **usernames = calloc(200, sizeof(byte));
    int users = GetAllLoggedInSocks5Users(usernames,200 );
    if(users == 0){
        free(usernames);
        fillBuffer("+NO USERS\r\n", buffer,11 );
        return 11;
    }


    users = (users == -1)? 200:users;
    //       +     usernames    pipes and \r    \n
    char str[1 +   users*255  +    users       + 1];
    str[0] = '+';
    str[1] = '\0';
    for (int i = 0; i < users; ++i) {
        strcat(str,usernames[i]);
        strcat(str,(i==users-1)? "\r\n":"|");
    }
    if (length < (strlen(str)) ) {
        Warning("Buffer to small to WriteHead ClientSetUserSizeResponse");
        free(usernames);
        return 0;
    }
    fillBuffer(str, buffer,(int) (strlen(str)) );
    free(usernames);
    return (strlen(str));
}

size_t BuildClientSetUserResponse(byte *buffer, size_t length, char* username, char* password) {

    if(username == null || password == null)
        return 0;

//    todo: free?/listo?

//  I check that username doesnt exist
    char **loggedUsernames = calloc(200, sizeof(byte));
    int users = GetAllSocks5Users(loggedUsernames,200 );

    users = (users == -1)? 200:users;

    for (int i = 0; i < users; ++i) {
        if(strcmp(loggedUsernames[i], username) == 0){
            if (length < LEN_USER_EXISTS) {
                Warning("Buffer to small to WriteHead ClientSetUserSizeResponse");
                return 0;
            }
            fillBuffer("-USER EXISTS\r\n", buffer,LEN_USER_EXISTS );
            return 14;
        }
    }
    free(loggedUsernames);
//    TODO: free?/listo
    LoadSingleUser(username,password );
    if (length < LEN_OK) {
        Warning("Buffer to small to WriteHead ClientSetUserSizeResponse");
        return 0;
    }
    fillBuffer("+OK\r\n", buffer,LEN_OK );
//    free(usernames);
//    free(passwords);
    return (LEN_OK);
}

size_t BuildClientDelUserResponse(byte *buffer, size_t length, char* username) {
    int status = DeleteSocks5User(username);
    switch (status) {
        case OK:
            if (length < LEN_OK) {
                Warning("Buffer to small to WriteHead ClientDelUserSizeResponse");
                return 0;
            }
            fillBuffer("+OK\r\n", buffer,LEN_OK );

            return (LEN_OK);
        case LOGGED_IN:
            if (length < LEN_USER_LOGGED_IN) {
                Warning("Buffer to small to WriteHead ClientDelUserSizeResponse");
                return 0;
            }
            fillBuffer("-USER LOGGED IN\r\n", buffer,LEN_USER_LOGGED_IN );

            return (LEN_USER_LOGGED_IN);
        case DOESNT_EXIST:
            if (length < LEN_USER_DOESNT_EXIST) {
                Warning("Buffer to small to WriteHead ClientDelUserSizeResponse");
                return 0;
            }
            fillBuffer("-USER DOESNT EXIST\r\n", buffer,LEN_USER_DOESNT_EXIST );

            return (LEN_USER_DOESNT_EXIST);
        default:
            break;

    }
    return -1;
}

size_t BuildClientNotRecognisedResponse(byte *buffer, size_t length) {

    if (length < LEN_NOT_RECOGNISED) {
        Warning("Buffer to small to WriteHead ClientNotRecognisedResponse");
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
