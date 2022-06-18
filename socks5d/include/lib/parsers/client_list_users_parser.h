//
// Created by tluci on 17/6/2022.
//

#ifndef SOCKS5D_CLIENT_LIST_USERS_PARSER_H
#define SOCKS5D_CLIENT_LIST_USERS_PARSER_H

#include "utils/utils.h"
#include <stdbool.h>
#include <string.h>
typedef enum {
    List,
    ListUsers,
    ListCRLF,
    ListFinished,
    ListInvalidState

} ClientListUsersParserState;

typedef struct {
    ClientListUsersParserState State;
    char Users[6];
    char List[5];
    uint8_t Index;
    char* Word;

} ClientListUsersParser;
bool ClientListUsersParserHasFinished(ClientListUsersParserState state);
void ClientListUsersParserReset(ClientListUsersParser *p);
ClientListUsersParserState ClientListUsersParserFeed(ClientListUsersParser *p, byte c);
size_t ClientListUsersParserConsume(ClientListUsersParser *p, byte *c, size_t length);
#endif //SOCKS5D_CLIENT_LIST_USERS_PARSER_H
