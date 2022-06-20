//
// Created by tluci on 19/6/2022.
//

#ifndef SOCKS5D_LULU_MESSAGES_H
#define SOCKS5D_LULU_MESSAGES_H

#include <stdbool.h>
#include <netinet/in.h>
#include "utils/utils.h"
#include "socks5/socks5_metrics.h"
#include "socks5/socks5_users.h"
#include "socks5/socks5_buffer.h"
#define LEN_GOODBYE 10
#define LEN_NOT_RECOGNISED 17
#define LEN_AUTHENTICATION_ERROR 23
#define LEN_OK 5
#define LEN_INVALID_BUFFER_SIZE 22
#define LEN_USER_EXISTS 14
#define LEN_USER_DOESNT_EXIST 20
#define LEN_USER_LOGGED_IN 17
size_t BuildClientHelloResponse(byte *buffer, size_t length, bool authenticationSucceeded);
size_t BuildClientGoodbyeResponse(byte *buffer, size_t length);
size_t BuildClientNotRecognisedResponse(byte *buffer, size_t length);
size_t BuildClientSetTimeoutResponse(byte *buffer, size_t length, size_t timeout);
size_t BuildClientSetBufferSizeResponse(byte *buffer, size_t length,int64_t bufferSize );
size_t BuildClientGetTimeoutResponse(byte *buffer, size_t length);
size_t BuildClientGetBufferSizeResponse(byte *buffer, size_t length);
size_t BuildClientGetMetricsResponse(byte *buffer, size_t length);
size_t BuildClientDelUserResponse(byte *buffer, size_t length, char* username);
size_t BuildClientSetUserResponse(byte *buffer, size_t length, char* username, char* password);
size_t BuildClientListUsersResponse(byte *buffer, size_t length);
#endif //SOCKS5D_LULU_MESSAGES_H
