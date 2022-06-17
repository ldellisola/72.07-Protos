//
// Created by tluci on 16/6/2022.
//

#ifndef SOCKS5D_CLIENT_TIMEOUT_PARSER_H
#define SOCKS5D_CLIENT_TIMEOUT_PARSER_H

#include "utils/utils.h"
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
typedef enum {
    TimeoutSet,
    Timeout,
    TimeoutValue,
    TimeoutCRLF,
    TimeoutFinished,
    TimeoutInvalidState

} ClientTimeoutParserState;

typedef struct {
    ClientTimeoutParserState State;
    char Timeout[8];
    char Set[4];
    uint8_t Value;
    uint8_t index;
    char* word;

} ClientTimeoutParser;
bool ClientTimeoutParserHasFinished(ClientTimeoutParserState state);
void ClientTimeoutParserReset(ClientTimeoutParser *p);
ClientTimeoutParserState ClientTimeoutParserFeed(ClientTimeoutParser *p, byte c);
size_t ClientTimeoutParserConsume(ClientTimeoutParser *p, byte *c, size_t length);
#endif //SOCKS5D_CLIENT_TIMEOUT_PARSER_H
