//
// Created by tluci on 16/6/2022.
//

#include "parsers/client_timeout_parser.h"
#include "utils/logger.h"

ClientTimeoutParserState traverseWordTimeout(ClientTimeoutParser *p, byte c, ClientTimeoutParserState nextState, char *nextWord) {

    if(strlen(p->word) == p->index){
        if(c == '|'){
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

void ClientTimeoutParserReset(ClientTimeoutParser *p) {
    LogInfo("Resetting ClientTimeoutParser...");
    if (null == p) {
        LogError(false, "Cannot reset NULL ClientTimeoutParser");
        return;
    }

    p->State = TimeoutSet;
    p->index = 0;
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
    p->word = p->Set;

    LogInfo("TimeoutParser reset!");
}

ClientTimeoutParserState ClientTimeoutParserFeed(ClientTimeoutParser *p, byte c) {
    LogInfo("Feeding %d to ClientTimeoutParser", c);
//    LogError(false, "char = %c", c);

    if (null == p) {
        LogError(false, "Cannot feed TimeoutParser if is NULL");
        return TimeoutInvalidState;
    }

    switch (p->State) {
        case TimeoutSet:
//            LogError(false, "TimeoutSet");
            p->State = traverseWordTimeout(p, c, Timeout, p->Timeout);
            break;
        case Timeout:
//            LogError(false, "Timeout");
            p->State = traverseWordTimeout(p, c, TimeoutValue, null);
            break;
        case TimeoutValue:
//            LogError(false, "TimeoutValue");
            if(isdigit(c)){
                p->Value *=10 + ((char)c- '0');
                break;
            }
            if(c == '\r'){
                p->State = TimeoutCRLF;
                break;
            }
            LogError(false, "Character is not a digit");
            p->State =TimeoutInvalidState;
            break;
        case TimeoutCRLF:
//            LogError(false, "TimeoutCRLF");
            p->State = c == '\n'? TimeoutFinished: TimeoutInvalidState;
            break;
        case TimeoutFinished:
//            LogError(false, "TimeoutFinished");
        case TimeoutInvalidState:
//            LogError(false, "TimeoutInvalidState");
            break;
    }
    return p->State;
}
size_t ClientTimeoutParserConsume(ClientTimeoutParser *p, byte *c, size_t length) {
    LogInfo("ClientTimeoutParser consuming %d bytes", length);
    if (null == p) {
        LogError(false, "Cannot consume if ClientTimeoutParser is NULL");
        return 0;
    }

    if (null == c) {
        LogError(false, "ClientTimeoutParser cannot consume NULL array");
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
