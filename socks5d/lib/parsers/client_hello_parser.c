//
// Created by tluci on 14/6/2022.
//

#include "parsers/client_hello_parser.h"
#include "utils/logger.h"

ClientHelloParserState traverseWord(ClientHelloParser *p, byte c, ClientHelloParserState nextState, char *nextWord) {
//    LogError(false, "char c = %c", c);
    if(p->State == Hello){
        if (p->index == strlen(p->word)-1) {
            if (c == '|') {
                p->index = 0;
                p->word = nextWord;
                return nextState;
            }
            LogError(false, "Im in the last letter of the word and there is no pipe");
            return HelloInvalidState;
        }

        // CASO SEGUIR: el caracter es parte de la palabra, seguir
        if (c == p->word[p->index]) {
            p->index++;
            return p->State;
        }
        LogError(false, "wrong character for HELLO, i was waiting for %c and got %c", p->word[p->index], c);
        return HelloInvalidState;
    }
    if(p->State == HelloCRLF){
        if(c == '\n'){
            if(p->PrevState == HelloPassword){
                return nextState;
            }
            LogError(false, "Arguments are missing");
            return HelloInvalidState;
        }
        p->State = p->PrevState;
        p->word[p->index] = '\r';
        p->index ++;
        p->word[p->index] = (char)c;
    }

    if(c == '|'){
        p->index = 0;
        p->word = nextWord;
        return nextState;
    }
    if(c == '\r'){
        p->PrevState = p->State;
        return HelloCRLF;
    }
    p->word[p->index] = (char)c;

    return p->State;

}

void ClientHelloParserReset(ClientHelloParser *p) {
    LogInfo("Resetting HelloParser...");
    if (null == p) {
        LogError(false, "Cannot reset NULL HelloParser");
        return;
    }

    p->State = Hello;
    p->index = 0;

    memset(p->UName, 0, 51);
    memset(p->Passwd, 0, 51);
    p->Hello[0] = 'H';
    p->Hello[1] = 'E';
    p->Hello[2] = 'L';
    p->Hello[3] = 'L';
    p->Hello[4] = 'O';
    p->word = p->Hello;
    LogInfo("HelloParser reset!");
}

ClientHelloParserState ClientHelloParserFeed(ClientHelloParser *p, byte c) {
    LogInfo("Feeding %d to ClientHelloParser", c);
    if (null == p) {
        LogError(false, "Cannot feed HelloParser if is NULL");
        return HelloInvalidState;
    }

    switch (p->State) {
        case Hello:
            p->State = traverseWord(p, c, HelloUsername, p->UName);
            break;
        case HelloUsername:
            p->State = traverseWord(p, c, HelloPassword, p->Passwd);
            break;
        case HelloPassword:
            p->State = traverseWord(p, c, HelloCRLF, "\r\n");
            break;
        case HelloCRLF:
            p->State = traverseWord(p, c, HelloFinished, null);
            break;
        case HelloFinished:
        case HelloInvalidState:
            break;
    }
    return p->State;
}

bool ClientHelloParserHasFailed(ClientHelloParserState state) {
    return state == HelloInvalidState ? true : false;
}

size_t ClientHelloParserConsume(ClientHelloParser *p, byte *c, size_t length) {
    LogInfo("AuthParser consuming %d bytes", length);
    if (null == p) {
        LogError(false, "Cannot consume if HelloParser is NULL");
        return 0;
    }

    if (null == c) {
        LogError(false, "HelloParser cannot consume NULL array");
        return 0;
    }

    for (size_t i = 0; i < length; ++i) {
        ClientHelloParserState state = ClientHelloParserFeed(p, c[i]);
        if (ClientHelloParserHasFinished(state))
            return i + 1;
    }
    return length;
}

bool ClientHelloParserHasFinished(ClientHelloParserState state) {
    switch (state) {
        default:
        case Hello:
        case HelloPassword:
        case HelloUsername:
            return false;
        case HelloInvalidState:
        case HelloFinished:
            return true;
    }
}
