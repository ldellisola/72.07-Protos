//
// Created by tluci on 17/6/2022.
//

#include "parsers/client_get_timeout_parser.h"
#include "utils/logger.h"

ClientGetTimeoutParserState traverseWordGetTimeout(ClientGetTimeoutParser *p, byte c, ClientGetTimeoutParserState nextState, char *nextWord) {

    if(strlen(p->word) == p->index){
        if((c == '|' && p->State == TimeoutGet) ||(c == '\r' && p->State == Timeout)){
            p->word = nextWord;
            p->index = 0;
            return nextState;
        }
        LogError(false, "The word has finished and character given isnt a terminating character");
        return TimeoutInvalidState;

    }

    if(c == p->word[p->index]){
        p->index++;
        return p->State;
    }
    LogError(false, "%c is not part of the word \" %s \"", c, p->word);
    return TimeoutInvalidState;
}

void ClientGetTimeoutParserReset(ClientGetTimeoutParser *p) {
    LogInfo("Resetting ClientGetTimeoutParser...");
    if (null == p) {
        LogError(false, "Cannot reset NULL ClientGetTimeoutParser");
        return;
    }

    p->State = TimeoutGet;
    p->index = 0;

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
    p->word = p->Get;

    LogInfo("TimeoutParser reset!");
}

ClientGetTimeoutParserState ClientGetTimeoutParserFeed(ClientGetTimeoutParser *p, byte c) {
    LogInfo("Feeding %d to ClientGetTimeoutParser", c);
//    LogError(false, "char = %c", c);

    if (null == p) {
        LogError(false, "Cannot feed GetTimeoutParser if is NULL");
        return TimeoutInvalidState;
    }

    switch (p->State) {
        case TimeoutGet:
//            LogError(false, "TimeoutGet");
            p->State = traverseWordGetTimeout(p, c, Timeout, p->Timeout);
            break;
        case Timeout:
//            LogError(false, "Timeout");
            p->State = traverseWordGetTimeout(p, c, TimeoutCRLF, null);
            break;

        case TimeoutCRLF:
//            LogError(false, "TimeoutCRLF");
            if( c == '\n'){
                p->State = TimeoutFinished;
                break;
            }
            LogError(false, "There is a CR but no LF");
            p->State =  TimeoutInvalidState;
            break;
        case TimeoutFinished:
//            LogError(false, "TimeoutFinished");
        case TimeoutInvalidState:
//            LogError(false, "TimeoutInvalidState");
            break;
    }
    return p->State;
}
size_t ClientGetTimeoutParserConsume(ClientGetTimeoutParser *p, byte *c, size_t length) {
    LogInfo("ClientTimeoutParser consuming %d bytes", length);
    if (null == p) {
        LogError(false, "Cannot consume if ClientGetTimeoutParser is NULL");
        return 0;
    }

    if (null == c) {
        LogError(false, "ClientGetTimeoutParser cannot consume NULL array");
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

