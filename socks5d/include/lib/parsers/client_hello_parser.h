//
// Created by tluci on 14/6/2022.
//

#ifndef SOCKS5D_CLIENT_HELLO_PARSER_H
#define SOCKS5D_CLIENT_HELLO_PARSER_H

#include "utils/utils.h"
#include <string.h>
#include <stdbool.h>

#define CR 0x0D
#define LF 0x0A

typedef enum {
    Hello,
    HelloUsername,
    HelloPassword,
    HelloFinished,
    HelloInvalidState

}HelloParserState;

typedef struct {
    HelloParserState State;
    //TODO: checkear que cuando termina la palabra que este en 0
    char UName[51];
    char Passwd[51];
    uint8_t index;
    char* word;

}HelloParser;

HelloParserState HelloParserFeed(HelloParser *p, byte c);
void HelloParserReset(HelloParser *p);
ssize_t HelloParserConsume(HelloParser *p, byte *c, ssize_t length);
bool HelloParserHasFailed(HelloParserState state);
bool HelloParserHasFinished(HelloParserState state);
#endif //SOCKS5D_CLIENT_HELLO_PARSER_H

