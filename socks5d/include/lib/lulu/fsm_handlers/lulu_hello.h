//
// Created by tluci on 19/6/2022.
//

#ifndef SOCKS5D_LULU_HELLO_H
#define SOCKS5D_LULU_HELLO_H
#include "parsers/lulu/client_hello_parser.h"
#include "parsers/lulu/client_goodbye_parser.h"
#include "utils/buffer.h"
#include "lulu/lulu_connection_status.h"
#define PARSER_COUNT 2
#define NO_RETURN -1
#define HELLO_PARSER 0
#define GOODBYE_PARSER 1
typedef struct {
    uint8_t ParserIndex;
    ClientHelloParser HelloParser;
    ClientGoodbyeParser GoodbyeParser;
    ArrayBuffer *WriteBuffer, *ReadBuffer;
    bool ClientHelloSucceeded;
} ClientHelloData;


void LuluHelloReadInit(unsigned state, void *data);
unsigned LuluHelloReadRun(void *data);
void LuluHelloReadClose(unsigned int state, void *data);
unsigned LuluHelloWriteRun(void *data);
void LuluHelloWriteClose(unsigned int state, void *data);
#endif //SOCKS5D_LULU_HELLO_H
