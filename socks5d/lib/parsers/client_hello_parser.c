//
// Created by tluci on 14/6/2022.
//

#include "parsers/client_hello_parser.h"
#include "utils/logger.h"

ClientHelloParserState traverseWord(ClientHelloParser *p, byte c, ClientHelloParserState nextState, char *nextWord) {
//    Error( "char c = %c", c);
    if(strlen(p->Word) == p->Index){
        if(c == '|'){
            p->Word = nextWord;
            p->Index = 0;
            return nextState;
        }
        Error( "Im in the last letter of the word and there is no pipe");
        return HelloInvalidState;
    }
    if (c == p->Word[p->Index]) {
        p->Index++;
        return p->State;
    }
    LogError( "wrong character for HELLO, i was waiting for %c and got %c", p->Word[p->Index], c);
    return HelloInvalidState;

}

void ClientHelloParserReset(ClientHelloParser *p) {
    Debug("Resetting HelloParser...");
    if (null == p) {
        Error( "Cannot reset NULL HelloParser");
        return;
    }

    p->State = Hello;
    p->Index = 0;

    memset(p->UName, 0, MAXLONG);
    memset(p->Passwd, 0, MAXLONG);
    p->Hello[0] = 'H';
    p->Hello[1] = 'E';
    p->Hello[2] = 'L';
    p->Hello[3] = 'L';
    p->Hello[4] = 'O';
    p->Hello[5] = 0;
    p->Word = p->Hello;
    Debug("HelloParser reset!");
}

ClientHelloParserState ClientHelloParserFeed(ClientHelloParser *p, byte c) {
    LogDebug("Feeding %d to ClientHelloParser", c);
//    Error( "char= %c", c);
    if (null == p) {
        Error( "Cannot feed HelloParser if is NULL");
        return HelloInvalidState;
    }

    switch (p->State) {
        case Hello:
            p->State = traverseWord(p, c, HelloUsername, p->UName);
            break;
        case HelloUsername:
            if(c == '|'){
                if(p->Index == 0){
                    Error( "Username has to be at least 1 character long");
                    p->State = HelloInvalidState;
                    break;
                }
                p->State = HelloPassword;
                p->Word = p->Passwd;
                p->Index = 0;
                break;
            }
            if(c == '\r'){
                p->PrevState = HelloUsername;
                p->State = HelloCRLF;
                break;
            }
            if(p->Index == MAXLONG+1){
                Error( "Username can have max 255 characters");
                p->State = HelloInvalidState;
                break;
            }
            p->Word[p->Index] = (char)c;
            p->Index++;
            break;
        case HelloPassword:
            if(c == '|'){
                Error( "Too many arguments");
                p->State = HelloInvalidState;
                break;
            }
            if(c == '\r'){
                p->PrevState = HelloPassword;
                p->State = HelloCRLF;
                break;
            }
            if(p->Index == MAXLONG+1){
                Error( "Password can have max 255 characters");
                p->State = HelloInvalidState;
                break;
            }
            p->Word[p->Index] = (char)c;
            p->Index++;
            break;
        case HelloCRLF:
            if(c == '\n'){
                if(p->PrevState == HelloPassword){
                    if(p->Index == 0){
                        p->State = HelloInvalidState;
                        Error( "Password has to be at least 1 character long");
                        break;
                    }
                    p->State = HelloFinished;
                    break;
                }
                Error( "More arguments needed");
                p->State = HelloInvalidState;
                break;
            }
            if(c == '\r'){
                if(p->Index == MAXLONG+1){
                    Error( "Password and Username can have max 255 characters");
                    p->State = HelloInvalidState;
                    break;
                }
                p->Word[p->Index] = '\r';
                p->Index++;
                break;
            }
            if(c == '|'){
                if(p->PrevState == HelloUsername){
                    if(p->Index == MAXLONG+1){
                        Error( "Username can have max 255 characters");
                        p->State = HelloInvalidState;
                        break;
                    }
                    p->Word[p->Index] = '\r';
                    p->Word = p->Passwd;
                    p->Index = 0;
                    p->State = HelloPassword;
                    break;
                }
                Error( "Too many arguments");
                p->State = HelloInvalidState;
                break;
            }
            if(p->Index == MAXLONG){
                Error( "Username and password can have max 255 characters");
                p->State = HelloInvalidState;
                break;
            }

            p->Word[p->Index] = '\r';
            p->Index++;
            p->Word[p->Index] = (char)c;
            p->Index++;
            p->State = p->PrevState;
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
    LogDebug("AuthParser consuming %d bytes", length);
    if (null == p) {
        Error( "Cannot consume if HelloParser is NULL");
        return 0;
    }

    if (null == c) {
        Error( "HelloParser cannot consume NULL array");
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
