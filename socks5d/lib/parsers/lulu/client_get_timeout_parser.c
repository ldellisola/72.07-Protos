//
// Created by tluci on 17/6/2022.
//

#include "parsers/lulu/client_get_timeout_parser.h"
#include "utils/logger.h"

ClientGetTimeoutParserState traverseWordGetTimeout(ClientGetTimeoutParser *p, byte c, ClientGetTimeoutParserState nextState, char *nextWord) {

    if(strlen(p->Word) == p->Index){
        if((c == '|' && p->State == GetTimeoutGet) ||(c == '\r' && p->State == GetTimeout)){
            p->Word = nextWord;
            p->Index = 0;
            return nextState;
        }
        Debug("The word has finished and character given isnt a terminating character");
        return GetTimeoutInvalidState;

    }

    if(c == p->Word[p->Index]){
        p->Index++;
        return p->State;
    }
    LogDebug(false, "%c is not part of the word \" %s \"", c, p->Word);
    return GetTimeoutInvalidState;
}

void ClientGetTimeoutParserReset(ClientGetTimeoutParser *p) {
    Debug("Resetting ClientGetTimeoutParser...");
    if (null == p) {
        Error("Cannot reset NULL ClientGetTimeoutParser");
        return;
    }

    p->State = GetTimeoutGet;
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
        LogDebug(false, "Cannot feed GetTimeoutParser if is NULL");
        return GetTimeoutInvalidState;
    }

    switch (p->State) {
        case GetTimeoutGet:
//            LogError(false, "TimeoutGet");
            p->State = traverseWordGetTimeout(p, c, GetTimeout, p->Timeout);
            break;
        case GetTimeout:
//            LogError(false, "Timeout");
            p->State = traverseWordGetTimeout(p, c, GetTimeoutCRLF, null);
            break;

        case GetTimeoutCRLF:
//            LogError(false, "TimeoutCRLF");
            if( c == '\n'){
                p->State = GetTimeoutFinished;
                break;
            }
            Debug("There is a CR but no LF");
            p->State =  GetTimeoutInvalidState;
            break;
        case GetTimeoutFinished:
//            LogError(false, "TimeoutFinished");
        case GetTimeoutInvalidState:
//            LogError(false, "TimeoutInvalidState");
            break;
    }
    return p->State;
}




size_t ClientGetTimeoutParserConsume(ClientGetTimeoutParser *p, byte *c, size_t length) {
    LogDebug("ClientTimeoutParser consuming %d bytes", length);
    if (null == p) {
        Debug( "Cannot consume if ClientGetTimeoutParser is NULL");
        return 0;
    }

    if (null == c) {
        Debug( "ClientGetTimeoutParser cannot consume NULL array");
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
        case GetTimeoutGet:
        case GetTimeout:
        case GetTimeoutCRLF:
            return false;
        case GetTimeoutInvalidState:
        case GetTimeoutFinished:
            return true;
    }
}

bool ClientGetTimeoutParserHasFailed(ClientGetTimeoutParserState state) {
    return state == GetTimeoutInvalidState ? true : false;
}


