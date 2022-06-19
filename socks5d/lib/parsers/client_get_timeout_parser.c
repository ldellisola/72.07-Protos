//
// Created by tluci on 17/6/2022.
//

#include "parsers/client_get_timeout_parser.h"
#include "utils/logger.h"

ClientGetTimeoutParserState traverseWordGetTimeout(ClientGetTimeoutParser *p, byte c, ClientGetTimeoutParserState nextState, char *nextWord) {

    if(strlen(p->Word) == p->Index){
        if((c == '|' && p->State == TimeoutGet) ||(c == '\r' && p->State == Timeout)){
            p->Word = nextWord;
            p->Index = 0;
            return nextState;
        }
        Error("The word has finished and character given isnt a terminating character");
        return TimeoutInvalidState;

    }

    if(c == p->Word[p->Index]){
        p->Index++;
        return p->State;
    }
    LogError( "%c is not part of the word \" %s \"", c, p->Word);
    return TimeoutInvalidState;
}

void ClientGetTimeoutParserReset(ClientGetTimeoutParser *p) {
    Debug("Resetting ClientGetTimeoutParser...");
    if (null == p) {
        Error("Cannot reset NULL ClientGetTimeoutParser");
        return;
    }

    p->State = TimeoutGet;
    p->Index = 0;

    p->Timeout[0] = 'T';
    p->Timeout[1] = 'I';
    p->Timeout[2] = 'M';
    p->Timeout[3] = 'E';
    p->Timeout[4] = 'O';
    p->Timeout[5] = 'U';
    p->Timeout[6] = 'T';
    p->Timeout[7] = 0;

    p->Get[0] = 'G';
    p->Get[1] = 'E';
    p->Get[2] = 'T';
    p->Get[3] = 0;
    p->Word = p->Get;

    Debug("TimeoutParser reset!");
}

ClientGetTimeoutParserState ClientGetTimeoutParserFeed(ClientGetTimeoutParser *p, byte c) {
    LogDebug("Feeding %d to ClientGetTimeoutParser", c);
//    Error( "char = %c", c);

    if (null == p) {
        Error("Cannot feed GetTimeoutParser if is NULL");
        return TimeoutInvalidState;
    }

    switch (p->State) {
        case TimeoutGet:
//            Error( "TimeoutGet");
            p->State = traverseWordGetTimeout(p, c, Timeout, p->Timeout);
            break;
        case Timeout:
//            Error( "Timeout");
            p->State = traverseWordGetTimeout(p, c, TimeoutCRLF, null);
            break;

        case TimeoutCRLF:
//            Error( "TimeoutCRLF");
            if( c == '\n'){
                p->State = TimeoutFinished;
                break;
            }
            Error("There is a CR but no LF");
            p->State =  TimeoutInvalidState;
            break;
        case TimeoutFinished:
//            Error( "TimeoutFinished");
        case TimeoutInvalidState:
//            Error( "TimeoutInvalidState");
            break;
    }
    return p->State;
}
size_t ClientGetTimeoutParserConsume(ClientGetTimeoutParser *p, byte *c, size_t length) {
    LogDebug("ClientTimeoutParser consuming %d bytes", length);
    if (null == p) {
        Error( "Cannot consume if ClientGetTimeoutParser is NULL");
        return 0;
    }

    if (null == c) {
        Error( "ClientGetTimeoutParser cannot consume NULL array");
        return 0;
    }

    for (size_t i = 0; i < length; ++i) {
        ClientGetTimeoutParserState state = ClientGetTimeoutParserFeed(p, c[i]);
        if (ClientGetTimeoutParserHasFinished(state))
            return i + 1;
    }
    return length;
}

bool ClientGetTimeoutParserHasFinished(ClientGetTimeoutParserState state) {
    switch (state) {
        default:
        case TimeoutGet:
        case Timeout:
        case TimeoutCRLF:
            return false;
        case TimeoutInvalidState:
        case TimeoutFinished:
            return true;
    }
}

