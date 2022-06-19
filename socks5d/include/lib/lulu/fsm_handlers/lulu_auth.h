//
// Created by tluci on 19/6/2022.
//

#ifndef SOCKS5D_LULU_AUTH_H
#define SOCKS5D_LULU_AUTH_H
#include "parsers/lulu/client_hello_parser.h"
#include "utils/buffer.h"

typedef struct {
    ClientHelloParser Parser;
    ArrayBuffer *WriteBuffer, *ReadBuffer;
    bool AuthSucceeded;
} AuthData;


void AuthReadInit(unsigned state, void *data);
#endif //SOCKS5D_LULU_AUTH_H
