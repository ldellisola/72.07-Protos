//
// Created by tluci on 17/6/2022.
//

#include "parsers/client_get_buffersize_parser.h"
#include "utils/logger.h"

ClientGetBufferSizeParserState traverseWordGetBufferSize(ClientGetBufferSizeParser *p, byte c, ClientGetBufferSizeParserState nextState, char *nextWord) {

    if(strlen(p->Word) == p->Index){
        if((c == '|' && p->State == BufferSizeGet) ||(c == '\r' && p->State == BufferSize)){
            p->Word = nextWord;
            p->Index = 0;
            return nextState;
        }
        LogError(false, "The word has finished and character given isnt a terminating character");
        return BufferSizeInvalidState;

    }

    if(c == p->Word[p->Index]){
        p->Index++;
        return p->State;
    }
    LogError(false, "%c is not part of the word \" %s \"", c, p->Word);
    return BufferSizeInvalidState;
}

void ClientGetBufferSizeParserReset(ClientGetBufferSizeParser *p) {
    LogInfo("Resetting ClientGetBufferSizeParser...");
    if (null == p) {
        LogError(false, "Cannot reset NULL ClientGetBufferSizeParser");
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

    LogInfo("BufferSizeParser reset!");
}

ClientGetBufferSizeParserState ClientGetBufferSizeParserFeed(ClientGetBufferSizeParser *p, byte c) {
    LogInfo("Feeding %d to ClientGetBufferSizeParser", c);
//    LogError(false, "char = %c", c);

    if (null == p) {
        LogError(false, "Cannot feed GetBufferSizeParser if is NULL");
        return BufferSizeInvalidState;
    }

    switch (p->State) {
        case BufferSizeGet:
//            LogError(false, "BufferSizeGet");
            p->State = traverseWordGetBufferSize(p, c, BufferSize, p->BufferSize);
            break;
        case BufferSize:
//            LogError(false, "BufferSize");
            p->State = traverseWordGetBufferSize(p, c, BufferSizeCRLF, null);
            break;

        case BufferSizeCRLF:
//            LogError(false, "BufferSizeCRLF");
            if( c == '\n'){
                p->State = BufferSizeFinished;
                break;
            }
            LogError(false, "There is a CR but no LF");
            p->State =  BufferSizeInvalidState;
            break;
        case BufferSizeFinished:
//            LogError(false, "BufferSizeFinished");
        case BufferSizeInvalidState:
//            LogError(false, "BufferSizeInvalidState");
            break;
    }
    return p->State;
}
size_t ClientGetBufferSizeParserConsume(ClientGetBufferSizeParser *p, byte *c, size_t length) {
    LogInfo("ClientBufferSizeParser consuming %d bytes", length);
    if (null == p) {
        LogError(false, "Cannot consume if ClientGetBufferSizeParser is NULL");
        return 0;
    }

    if (null == c) {
        LogError(false, "ClientGetBufferSizeParser cannot consume NULL array");
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
