//
// Created by tluci on 19/6/2022.
//

#ifndef SOCKS5D_LULU_MESSAGES_H
#define SOCKS5D_LULU_MESSAGES_H

#include <stdbool.h>
#include <netinet/in.h>
#include "utils/utils.h"
#define LEN_GOODBYE 10
#define LEN_HELLO 5
#define LEN_AUTHENTICATION_ERROR 23
#define LEN_OK 5
size_t BuildClientHelloResponse(byte *buffer, size_t length, bool authenticationSucceeded);
size_t BuildClientGoodbyeResponse(byte *buffer, size_t length);

#endif //SOCKS5D_LULU_MESSAGES_H
