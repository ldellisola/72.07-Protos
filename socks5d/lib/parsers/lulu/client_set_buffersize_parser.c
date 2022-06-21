//
// Created by tluci on 17/6/2022.
//

#include "parsers/lulu/client_set_buffersize_parser.h"
#include "utils/logger.h"

ClientSetBufferSizeParserState traverseWordSetBufferSize(ClientSetBufferSizeParser *p, byte c, ClientSetBufferSizeParserState nextState, char *nextWord) {

    if(strlen(p->Word) == p->Index){
        if(c == '|'){
            p->Word = nextWord;
            p->Index = 0;
            return nextState;
        }
        Debug("The word has finished and character given isnt a terminating character");
        return SetBufferSizeInvalidState;

    }

    if(c == p->Word[p->Index]){
        p->Index++;
        return p->State;
    }
    LogDebug("%c is not part of the word \" %s \"", c, p->Word);
    return SetBufferSizeInvalidState;
}

void ClientSetBufferSizeParserReset(ClientSetBufferSizeParser *p) {
    Debug("Resetting ClientSetBufferSizeParser...");
    if (null == p) {
        Debug( "Cannot reset NULL ClientSetBufferSizeParser");
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

    Debug("SetBufferSizeParser reset!");
}

ClientSetBufferSizeParserState ClientSetBufferSizeParserFeed(ClientSetBufferSizeParser *p, byte c) {
    LogDebug("Feeding %d to ClientSetBufferSizeParser", c);
//    Error( "char = %c", c);

    if (null == p) {
        Debug( "Cannot feed SetBufferSizeParser if is NULL");
        return SetBufferSizeInvalidState;
    }

    switch (p->State) {
        case BufferSizeSet:
//            LogError(false, "BufferSizeSet");
            p->State = traverseWordSetBufferSize(p, c, SetBufferSize, p->BufferSize);
            break;
        case SetBufferSize:
            p->State = traverseWordSetBufferSize(p, c, BufferSizeValue, null);
            break;
        case BufferSizeValue:
            if(isdigit(c)){
                int digit = (uint8_t) ((char)c- '0');
                p->Value = (p->Value * 10) + digit;
                if(p->Value > 10000000000){
                    LogDebug("BufferSize too big", p->Value, digit);
                    p->State = SetBufferSizeInvalidState;
                }
                break;
            }
            if(c == '\r'){
                p->State = SetBufferSizeCRLF;
                break;
            }
            LogDebug(false, "Character\"%c\" is not a digit", c);
            p->State =SetBufferSizeInvalidState;
            break;
        case SetBufferSizeCRLF:
            if( c == '\n'){
                p->State = SetBufferSizeFinished;
                break;
            }
            LogDebug(false, "Waiting for LF for CRLF pair and got: %c", c);
            p->State = SetBufferSizeInvalidState;
            break;
        case SetBufferSizeFinished:
        case SetBufferSizeInvalidState:
            break;
    }
    return p->State;
}
size_t ClientSetBufferSizeParserConsume(ClientSetBufferSizeParser *p, byte *c, size_t length) {
    LogDebug("ClientSetBufferSizeParser consuming %d bytes", length);
    if (null == p) {
        Debug( "Cannot consume if ClientSetBufferSizeParser is NULL");
        return 0;
    }

    if (null == c) {
        Debug( "ClientSetBufferSizeParser cannot consume NULL array");
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
        case SetBufferSize:
        case BufferSizeValue:
        case SetBufferSizeCRLF:
            return false;
        case SetBufferSizeFinished:
        case SetBufferSizeInvalidState:
            return true;
    }
}
bool ClientSetBufferSizeParserHasFailed(ClientSetBufferSizeParserState state) {
    return state == SetBufferSizeInvalidState ? true : false;
}
