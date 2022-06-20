//
// Created by tluci on 17/6/2022.
//

#ifndef SOCKS5D_CLIENT_GET_BUFFERSIZE_PARSER_H
#define SOCKS5D_CLIENT_GET_BUFFERSIZE_PARSER_H

#include "utils/utils.h"
#include <string.h>
#include <stdbool.h>

typedef enum {
    BufferSizeGet,
    BufferSize,
    BufferSizeCRLF,
    BufferSizeFinished,
    BufferSizeInvalidState

} ClientGetBufferSizeParserState;

typedef struct {
    ClientGetBufferSizeParserState State;
    char BufferSize[11];
    char Get[4];
    uint8_t Index;
    char* Word;

} ClientGetBufferSizeParser;
bool ClientGetBufferSizeParserHasFinished(ClientGetBufferSizeParserState state);
void ClientGetBufferSizeParserReset(ClientGetBufferSizeParser *p);
ClientGetBufferSizeParserState ClientGetBufferSizeParserFeed(ClientGetBufferSizeParser *p, byte c);
size_t ClientGetBufferSizeParserConsume(ClientGetBufferSizeParser *p, byte *c, size_t length);
bool ClientGetBufferSizeHasFailed(ClientGetBufferSizeParserState state);
#endif //SOCKS5D_CLIENT_GET_BUFFERSIZE_PARSER_H
