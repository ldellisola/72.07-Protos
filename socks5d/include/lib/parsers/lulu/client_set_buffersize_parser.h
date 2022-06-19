//
// Created by tluci on 17/6/2022.
//

#ifndef SOCKS5D_CLIENT_SET_BUFFERSIZE_PARSER_H
#define SOCKS5D_CLIENT_SET_BUFFERSIZE_PARSER_H

#include "utils/utils.h"
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
typedef enum {
    BufferSizeSet,
    SetBufferSize,
    BufferSizeValue,
    SetBufferSizeCRLF,
    SetBufferSizeFinished,
    SetBufferSizeInvalidState

} ClientSetBufferSizeParserState;

typedef struct {
    ClientSetBufferSizeParserState State;
    char BufferSize[11];
    char Set[4];
    uint64_t Value;
    uint8_t Index;
    char* Word;

} ClientSetBufferSizeParser;

bool ClientSetBufferSizeParserHasFinished(ClientSetBufferSizeParserState state);
void ClientSetBufferSizeParserReset(ClientSetBufferSizeParser *p);
ClientSetBufferSizeParserState ClientSetBufferSizeParserFeed(ClientSetBufferSizeParser *p, byte c);
size_t ClientSetBufferSizeParserConsume(ClientSetBufferSizeParser *p, byte *c, size_t length);

#endif //SOCKS5D_CLIENT_SET_BUFFERSIZE_PARSER_H
