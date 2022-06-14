//
// Created by Lucas Dell'Isola on 13/06/2022.
//

#ifndef SOCKS5D_SOCKS5_AUTH_H
#define SOCKS5D_SOCKS5_AUTH_H

#include "parsers/auth_parser.h"
#include "utils/buffer.h"

typedef struct {
    AuthParser Parser;
    ArrayBuffer * WriteBuffer, * ReadBuffer;
    bool AuthSucceeded;
} AuthData;


void AuthReadInit(unsigned state, void * data);

void AuthReadClose(unsigned state, void * data);

unsigned AuthReadRun(void * data);

void AuthWriteClose(unsigned state, void * data);

unsigned AuthWriteRun(void * data);

#endif //SOCKS5D_SOCKS5_AUTH_H
