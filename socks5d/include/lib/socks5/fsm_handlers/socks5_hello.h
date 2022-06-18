//
// Created by Lucas Dell'Isola on 09/06/2022.
//

#ifndef SOCKS5D_SOCKS5_HELLO_H
#define SOCKS5D_SOCKS5_HELLO_H

#include "parsers/socks5/hello_parser.h"
#include "utils/buffer.h"

typedef struct {
    HelloParser Parser;
    ArrayBuffer *WriteBuffer, *ReadBuffer;
    byte AuthenticationMethod;
} HelloData;


void HelloReadInit(unsigned state, void *data);

void HelloReadClose(unsigned state, void *data);

unsigned HelloReadRun(void *data);

unsigned HelloWriteRun(void *data);

void HelloWriteClose(unsigned int state, void *data);

#endif //SOCKS5D_SOCKS5_HELLO_H

