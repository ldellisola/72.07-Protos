//
// Created by tluci on 17/6/2022.
//

#ifndef SOCKS5D_CLIENT_GET_TIMEOUT_PARSER_H
#define SOCKS5D_CLIENT_GET_TIMEOUT_PARSER_H

#include "utils/utils.h"
#include <string.h>
#include <stdbool.h>

typedef enum {
    TimeoutGet,
    Timeout,
    TimeoutCRLF,
    TimeoutFinished,
    TimeoutInvalidState

} ClientGetTimeoutParserState;

typedef struct {
    ClientGetTimeoutParserState State;
    char Timeout[8];
    char Get[4];
    uint8_t Index;
    char* Word;

} ClientGetTimeoutParser;
bool ClientGetTimeoutParserHasFinished(ClientGetTimeoutParserState state);
void ClientGetTimeoutParserReset(ClientGetTimeoutParser *p);
ClientGetTimeoutParserState ClientGetTimeoutParserFeed(ClientGetTimeoutParser *p, byte c);
size_t ClientGetTimeoutParserConsume(ClientGetTimeoutParser *p, byte *c, size_t length);
bool ClientGetTimeoutParserHasFinished(ClientGetTimeoutParserState state);
#endif //SOCKS5D_CLIENT_GET_TIMEOUT_PARSER_H
