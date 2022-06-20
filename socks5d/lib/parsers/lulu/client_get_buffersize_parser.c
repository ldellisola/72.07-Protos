//
// Created by tluci on 17/6/2022.
//

#include "parsers/lulu/client_get_buffersize_parser.h"
#include "utils/logger.h"

#include <string.h>
ClientGetBufferSizeParserState traverseWordGetBufferSize(ClientGetBufferSizeParser *p, byte c, ClientGetBufferSizeParserState nextState, char *nextWord) {

    if(strlen(p->Word) == p->Index){
        if((c == '|' && p->State == BufferSizeGet) ||(c == '\r' && p->State == BufferSize)){
            p->Word = nextWord;
            p->Index = 0;
            return nextState;
        }
        Debug("The word has finished and character given isnt a terminating character");
        return BufferSizeInvalidState;

    }

    if(c == p->Word[p->Index]){
        p->Index++;
        return p->State;
    }
    LogDebug( "%c is not part of the word \" %s \"", c, p->Word);
    return BufferSizeInvalidState;
}

void ClientGetBufferSizeParserReset(ClientGetBufferSizeParser *p) {
    Debug("Resetting ClientGetBufferSizeParser...");
    if (null == p) {
        Debug("Cannot reset NULL ClientGetBufferSizeParser");
        return;
    }

    p->State = BufferSizeGet;
    p->Index = 0;

    p->BufferSize[0] = 'B';
    p->BufferSize[1] = 'U';
    p->BufferSize[2] = 'F';
    p->BufferSize[3] = 'F';
    p->BufferSize[4] = 'E';
    p->BufferSize[5] = 'R';
    p->BufferSize[6] = 'S';
    p->BufferSize[7] = 'I';
    p->BufferSize[8] = 'Z';
    p->BufferSize[9] = 'E';
    p->BufferSize[10] = 0;

    p->Get[0] = 'G';
    p->Get[1] = 'E';
    p->Get[2] = 'T';
    p->Get[3] = 0;
    p->Word = p->Get;

    Debug("BufferSizeParser reset!");
}

ClientGetBufferSizeParserState ClientGetBufferSizeParserFeed(ClientGetBufferSizeParser *p, byte c) {
    LogDebug("Feeding %d to ClientGetBufferSizeParser", c);

    if (null == p) {
        Debug("Cannot feed GetBufferSizeParser if is NULL");
        return BufferSizeInvalidState;
    }

    switch (p->State) {
        case BufferSizeGet:
//            Error( "BufferSizeGet");
            p->State = traverseWordGetBufferSize(p, c, BufferSize, p->BufferSize);
            break;
        case BufferSize:
//            Error( "BufferSize");
            p->State = traverseWordGetBufferSize(p, c, BufferSizeCRLF, null);
            break;

        case BufferSizeCRLF:
//            Error( "BufferSizeCRLF");
            if( c == '\n'){
                p->State = BufferSizeFinished;
                break;
            }
            Debug( "There is a CR but no LF");
            p->State =  BufferSizeInvalidState;
            break;
        case BufferSizeFinished:
//            Error( "BufferSizeFinished");
        case BufferSizeInvalidState:
//            Error( "BufferSizeInvalidState");
            break;
    }
    return p->State;
}
size_t ClientGetBufferSizeParserConsume(ClientGetBufferSizeParser *p, byte *c, size_t length) {
    LogDebug("ClientBufferSizeParser consuming %d bytes", length);
    if (null == p) {
        Debug( "Cannot consume if ClientGetBufferSizeParser is NULL");
        return 0;
    }

    if (null == c) {
        Debug( "ClientGetBufferSizeParser cannot consume NULL array");
        return 0;
    }

    for (size_t i = 0; i < length; ++i) {
        ClientGetBufferSizeParserState state = ClientGetBufferSizeParserFeed(p, c[i]);
        if (ClientGetBufferSizeParserHasFinished(state))
            return i + 1;
    }
    return length;
}

bool ClientGetBufferSizeParserHasFinished(ClientGetBufferSizeParserState state) {
    switch (state) {
        default:
        case BufferSizeGet:
        case BufferSize:
        case BufferSizeCRLF:
            return false;
        case BufferSizeInvalidState:
        case BufferSizeFinished:
            return true;
    }
}

bool ClientGetBufferSizeParserHasFailed(ClientGetBufferSizeParserState state){
    return state == BufferSizeInvalidState ? true : false;
}

