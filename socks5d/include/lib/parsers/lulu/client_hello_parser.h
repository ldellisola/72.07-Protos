//
// Created by tluci on 14/6/2022.
//

#ifndef SOCKS5D_CLIENT_HELLO_PARSER_H
#define SOCKS5D_CLIENT_HELLO_PARSER_H

#include <unistd.h>
#include "utils/utils.h"
#include <string.h>
#include <stdbool.h>

#define MAXLONG 255
typedef enum {
    Hello,
    HelloUsername,
    HelloPassword,
    HelloCRLF,
    HelloFinished,
    ClientHelloInvalidState

} ClientHelloParserState;

typedef struct {
    ClientHelloParserState State;
    ClientHelloParserState PrevState;
    char UName[MAXLONG];
    char Passwd[MAXLONG];
    //TODO: ponerle 0 al final
    char Hello[6];
    uint16_t Index;
    char* Word;

} ClientHelloParser;

ClientHelloParserState ClientHelloParserFeed(ClientHelloParser *p, byte c);

void ClientHelloParserReset(ClientHelloParser *p);

size_t ClientHelloParserConsume(ClientHelloParser *p, byte *c, size_t length);

bool ClientHelloParserHasFailed(ClientHelloParserState state);

bool ClientHelloParserHasFinished(ClientHelloParserState state);

#endif //SOCKS5D_CLIENT_HELLO_PARSER_H

