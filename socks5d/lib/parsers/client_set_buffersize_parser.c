//
// Created by tluci on 17/6/2022.
//

#include "parsers/client_set_buffersize_parser.h"
#include "utils/logger.h"

ClientSetBufferSizeParserState traverseWordSetBufferSize(ClientSetBufferSizeParser *p, byte c, ClientSetBufferSizeParserState nextState, char *nextWord) {

    if(strlen(p->Word) == p->Index){
        if(c == '|'){
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

void ClientSetBufferSizeParserReset(ClientSetBufferSizeParser *p) {
    LogInfo("Resetting ClientSetBufferSizeParser...");
    if (null == p) {
        LogError(false, "Cannot reset NULL ClientSetBufferSizeParser");
        return;
    }

    p->State = BufferSizeSet;
    p->Index = 0;
    p->Value = 0;

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

    p->Set[0] = 'S';
    p->Set[1] = 'E';
    p->Set[2] = 'T';
    p->Set[3] = 0;
    p->Word = p->Set;

    LogInfo("SetBufferSizeParser reset!");
}

ClientSetBufferSizeParserState ClientSetBufferSizeParserFeed(ClientSetBufferSizeParser *p, byte c) {
    LogInfo("Feeding %d to ClientSetBufferSizeParser", c);
//    LogError(false, "char = %c", c);

    if (null == p) {
        LogError(false, "Cannot feed SetBufferSizeParser if is NULL");
        return BufferSizeInvalidState;
    }

    switch (p->State) {
        case BufferSizeSet:
//            LogError(false, "BufferSizeSet");
            p->State = traverseWordSetBufferSize(p, c, BufferSize, p->BufferSize);
            break;
        case BufferSize:
//            LogError(false, "BufferSize");
            p->State = traverseWordSetBufferSize(p, c, BufferSizeValue, null);
            break;
        case BufferSizeValue:
//            LogError(false, "BufferSizeValue");
            if(isdigit(c)){
                int digit = (uint8_t) ((char)c- '0');
                p->Value = (p->Value * 10) + digit;
                if(p->Value > 10000000000){
                    LogError(false, "BufferSize too big", p->Value, digit);
                    p->State = BufferSizeInvalidState;
                }
//                LogError(false, "is digit, value = %d, digit = %d", p->Value, digit);
                break;
            }
            if(c == '\r'){
                p->State = BufferSizeCRLF;
                break;
            }
            LogError(false, "Character\"%c\" is not a digit", c);
            p->State =BufferSizeInvalidState;
            break;
        case BufferSizeCRLF:
//            LogError(false, "BufferSizeCRLF");
            if( c == '\n'){
                p->State = BufferSizeFinished;
                break;
            }
            LogError(false, "Waiting for LF for CRLF pair and got: %c", c);
            p->State = BufferSizeInvalidState;
            break;
        case BufferSizeFinished:
//            LogError(false, "BufferSizeFinished");
        case BufferSizeInvalidState:
//            LogError(false, "BufferSizeInvalidState");
            break;
    }
    return p->State;
}
size_t ClientSetBufferSizeParserConsume(ClientSetBufferSizeParser *p, byte *c, size_t length) {
    LogInfo("ClientSetBufferSizeParser consuming %d bytes", length);
    if (null == p) {
        LogError(false, "Cannot consume if ClientSetBufferSizeParser is NULL");
        return 0;
    }

    if (null == c) {
        LogError(false, "ClientSetBufferSizeParser cannot consume NULL array");
        return 0;
    }

    for (size_t i = 0; i < length; ++i) {
        ClientSetBufferSizeParserState state = ClientSetBufferSizeParserFeed(p, c[i]);
        if (ClientSetBufferSizeParserHasFinished(state))
            return i + 1;
    }
    return length;
}

bool ClientSetBufferSizeParserHasFinished(ClientSetBufferSizeParserState state) {
    switch (state) {
        default:
        case BufferSizeSet:
        case BufferSize:
        case BufferSizeValue:
        case BufferSizeCRLF:
            return false;
        case BufferSizeFinished:
        case BufferSizeInvalidState:
            return true;
    }
}
