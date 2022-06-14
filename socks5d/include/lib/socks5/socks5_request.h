//
// Created by Lucas Dell'Isola on 14/06/2022.
//

#ifndef SOCKS5D_SOCKS5_REQUEST_H
#define SOCKS5D_SOCKS5_REQUEST_H

#include "parsers/request_parser.h"
#include "utils/buffer.h"

typedef struct {
    RequestParser Parser;
    ArrayBuffer * WriteBuffer, * ReadBuffer;
    byte Command;
} RequestData;


void RequestReadInit(unsigned state, void * data);

void RequestReadClose(unsigned state, void * data);

unsigned RequestReadRun(void * data);

void RequestWriteClose(unsigned state, void * data);

unsigned RequestWriteRun(void * data);

void RequestWriteInit(unsigned int state, void *data);


#endif //SOCKS5D_SOCKS5_REQUEST_H
