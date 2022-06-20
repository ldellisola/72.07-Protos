//
// Created by tluci on 16/6/2022.
//

#include "parsers/lulu/client_timeout_parser.h"
#include "utils/logger.h"

ClientTimeoutParserState traverseWordTimeout(ClientTimeoutParser *p, byte c, ClientTimeoutParserState nextState, char *nextWord) {

    if(strlen(p->Word) == p->Index){
        if(c == '|'){
            p->Word = nextWord;
            p->Index = 0;
            return nextState;
        }
        Debug( "The word has finished and character given isnt a terminating character");
        return TimeoutInvalidState;

    }

    if(c == p->Word[p->Index]){
        p->Index++;
        return p->State;
    }
    LogDebug( "%c is not part of the word \" %s \"", c, p->Word);
    return TimeoutInvalidState;
}

void ClientTimeoutParserReset(ClientTimeoutParser *p) {
    Debug("Resetting ClientTimeoutParser...");
    if (null == p) {
        Debug( "Cannot reset NULL ClientTimeoutParser");
        return;
    }

    p->State = TimeoutSet;
    p->Index = 0;
    p->Value = 0;

    p->Timeout[0] = 'T';
    p->Timeout[1] = 'I';
    p->Timeout[2] = 'M';
    p->Timeout[3] = 'E';
    p->Timeout[4] = 'O';
    p->Timeout[5] = 'U';
    p->Timeout[6] = 'T';
    p->Timeout[7] = 0;

    p->Set[0] = 'S';
    p->Set[1] = 'E';
    p->Set[2] = 'T';
    p->Set[3] = 0;
    p->Word = p->Set;

    Debug("TimeoutParser reset!");
}

ClientTimeoutParserState ClientTimeoutParserFeed(ClientTimeoutParser *p, byte c) {
    LogDebug("Feeding %d to ClientTimeoutParser", c);
//    Error( "char = %c", c);

    if (null == p) {
        Debug( "Cannot feed TimeoutParser if is NULL");
        return TimeoutInvalidState;
    }

    switch (p->State) {
        case TimeoutSet:
//            Error( "TimeoutSet");
            p->State = traverseWordTimeout(p, c, Timeout, p->Timeout);
            break;
        case Timeout:
//            Error( "Timeout");
            p->State = traverseWordTimeout(p, c, TimeoutValue, null);
            break;
        case TimeoutValue:
//            Error( "TimeoutValue");
            if(isdigit(c)){
                p->Value *=10 + ((char)c- '0');
                break;
            }
            if(c == '\r'){
                p->State = TimeoutCRLF;
                break;
            }
            LogDebug( "Character\"%c\" is not a digit", c);
            p->State =TimeoutInvalidState;
            break;
        case TimeoutCRLF:
//            Error( "TimeoutCRLF");
            p->State = c == '\n'? TimeoutFinished: TimeoutInvalidState;
            break;
        case TimeoutFinished:
//            Error( "TimeoutFinished");
        case TimeoutInvalidState:
//            Error( "TimeoutInvalidState");
            break;
    }
    return p->State;
}
size_t ClientTimeoutParserConsume(ClientTimeoutParser *p, byte *c, size_t length) {
    LogDebug("ClientTimeoutParser consuming %d bytes", length);
    if (null == p) {
        Debug( "Cannot consume if ClientTimeoutParser is NULL");
        return 0;
    }

    if (null == c) {
        Debug( "ClientTimeoutParser cannot consume NULL array");
        return 0;
    }

    for (size_t i = 0; i < length; ++i) {
        ClientTimeoutParserState state = ClientTimeoutParserFeed(p, c[i]);
        if (ClientTimeoutParserHasFinished(state))
            return i + 1;
    }
    return length;
}

bool ClientTimeoutParserHasFinished(ClientTimeoutParserState state) {
    switch (state) {
        default:
        case TimeoutSet:
        case Timeout:
        case TimeoutValue:
        case TimeoutCRLF:
            return false;
        case TimeoutInvalidState:
        case TimeoutFinished:
            return true;
    }
}

bool ClientTimeoutParserHasFailed(ClientTimeoutParserState state) {
    return state == TimeoutInvalidState ? true : false;
}
