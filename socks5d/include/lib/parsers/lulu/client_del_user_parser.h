//
// Created by tluci on 17/6/2022.
//

#ifndef SOCKS5D_CLIENT_DEL_USER_PARSER_H
#define SOCKS5D_CLIENT_DEL_USER_PARSER_H

#include "utils/utils.h"
#include <string.h>
#include <stdbool.h>
#define MAXLONG 255
typedef enum {
    Del,
    DelUser,
    DelUserName,
    DelCRLF,
    DelFinished,
    DelInvalidState

} ClientDelUserParserState;

typedef struct {
    ClientDelUserParserState State;
    char Del[4];
    char User[5];
    char UName[MAXLONG];
    uint64_t Index;
    char* Word;

} ClientDelUserParser;
bool ClientDelUserParserHasFinished(ClientDelUserParserState state);
void ClientDelUserParserReset(ClientDelUserParser *p);
ClientDelUserParserState ClientDelUserParserFeed(ClientDelUserParser *p, byte c);
size_t ClientDelUserParserConsume(ClientDelUserParser *p, byte *c, size_t length);
bool ClientDelUserParserHasFailed(ClientDelUserParserState state);

#endif //SOCKS5D_CLIENT_DEL_USER_PARSER_H
