//
// Created by tluci on 17/6/2022.
//

#ifndef SOCKS5D_CLIENT_SET_USER_PARSER_H
#define SOCKS5D_CLIENT_SET_USER_PARSER_H

#include "utils/utils.h"
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#define MAXLONG 255
typedef enum {
    UserSet,
    User,
    UserName,
    UserPassword,
    UserCRLF,
    UserFinished,
    UserInvalidState

} ClientSetUserParserState;

typedef struct {
    ClientSetUserParserState State;
    ClientSetUserParserState PrevState;
    char User[5];
    char Set[4];
    char UName[MAXLONG];
    char Passwd[MAXLONG];
    uint16_t Index;
    char* Word;

} ClientSetUserParser;
bool ClientSetUserParserHasFinished(ClientSetUserParserState state);
void ClientSetUserParserReset(ClientSetUserParser *p);
ClientSetUserParserState ClientSetUserParserFeed(ClientSetUserParser *p, byte c);
size_t ClientSetUserParserConsume(ClientSetUserParser *p, byte *c, size_t length);
bool ClientSetUserParserHasFailed(ClientSetUserParserState state);
#endif //SOCKS5D_CLIENT_SET_USER_PARSER_H
