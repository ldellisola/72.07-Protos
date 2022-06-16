//
// Created by tluci on 14/6/2022.
//

#ifndef SOCKS5D_CLIENT_HELLO_PARSER_H
#define SOCKS5D_CLIENT_HELLO_PARSER_H

#include <unistd.h>
#include "utils/utils.h"
#include <string.h>
#include <stdbool.h>

#define CR 0x0D

typedef enum {
    Hello,
    HelloUsername,
    HelloPassword,
    HelloCRLF,
    HelloFinished,
    HelloInvalidState

} ClientHelloParserState;

typedef struct {
    ClientHelloParserState State;
    ClientHelloParserState PrevState;
    char UName[51];
    char Passwd[51];
    char Hello[5];
    uint8_t index;
    char* word;

} ClientHelloParser;

ClientHelloParserState ClientHelloParserFeed(ClientHelloParser *p, byte c);

void ClientHelloParserReset(ClientHelloParser *p);

size_t ClientHelloParserConsume(ClientHelloParser *p, byte *c, size_t length);

bool ClientHelloParserHasFailed(ClientHelloParserState state);

bool ClientHelloParserHasFinished(ClientHelloParserState state);

#endif //SOCKS5D_CLIENT_HELLO_PARSER_H

