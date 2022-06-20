//
// Created by tluci on 16/6/2022.
//

#ifndef SOCKS5D_CLIENT_GOODBYE_PARSER_H
#define SOCKS5D_CLIENT_GOODBYE_PARSER_H

#include "utils/utils.h"
#include <string.h>
#include <stdbool.h>
typedef enum {
    Goodbye,
    GoodbyeCRLF,
    GoodbyeFinished,
    GoodbyeInvalidState

} ClientGoodbyeParserState;
typedef struct {
    ClientGoodbyeParserState State;
    char Goodbye[7];
    uint8_t Index;

} ClientGoodbyeParser;
bool ClientGoodbyeParserHasFinished(ClientGoodbyeParserState state);
void ClientGoodbyeParserReset(ClientGoodbyeParser *p);
ClientGoodbyeParserState ClientGoodbyeParserFeed(ClientGoodbyeParser *p, byte c);
size_t ClientGoodbyeParserConsume(ClientGoodbyeParser *p, byte *c, size_t length);
bool ClientGoodbyeParserHasFailed(ClientGoodbyeParserState state);
#endif //SOCKS5D_CLIENT_GOODBYE_PARSER_H
