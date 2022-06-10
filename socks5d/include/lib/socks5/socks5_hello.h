//
// Created by Lucas Dell'Isola on 09/06/2022.
//

#ifndef SOCKS5D_SOCKS5_HELLO_H
#define SOCKS5D_SOCKS5_HELLO_H
#include "parsers/hello_parser.h"
#include "utils/buffer.h"

typedef struct {
    // TODO: Stop using a pointer to the parser
    HelloParser * Parser;
    ArrayBuffer * WriteBuffer, * ReadBuffer;
    byte AuthenticationMethod;
} HelloData;


void HelloReadInit(unsigned state, void * data);

void HelloReadClose(unsigned state, void * data);

unsigned HelloReadRun(void * data);

#endif //SOCKS5D_SOCKS5_HELLO_H
