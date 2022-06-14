//
// Created by tluci on 14/6/2022.
//

#include <unistd.h>
#include "parsers/client_hello_parser.h"
#include "utils/logger.h"

HelloParserState traverseWord(HelloParser *p, byte c, HelloParserState nextState, char* nextWord){
    // CASO TERMINAR: no hay siguiente argumento
    if(p->index == strlen(p->word) && nextWord == null){
        return c == CR ? nextState : HelloInvalidState;
    }

    // CASO TERMINAR: hay siguiente argumento
    if(p->index == strlen(p->word)){
        if( c == '|'){
            p->index = 0;
            strcpy(p->word, nextWord);
            return nextState;
        }
        return HelloInvalidState;

    }

    // CASO SEGUIR: el caracter es parte de la palabra, seguir
    if(c == p->word[p->index]){
        p->index++;
        return p->State;
    }
    // CASO MALO: el caracter no es parte de la palabra
    return HelloInvalidState;

}
void HelloParserReset(HelloParser *p) {
    LogInfo("Resetting HelloParser...");
    if (null == p) {
        LogError(false, "Cannot reset NULL HelloParser");
        return;
    }

    p->State = Hello;
    p->index = 0;
    strcpy(p->word, "HELLO");

    memset(p->UName,0,51);
    memset(p->Passwd,0,51);

    LogInfo("HelloParser reset!");
}

HelloParserState HelloParserFeed(HelloParser *p, byte c){
    LogInfo("Feeding %d to HelloParser", c);
    if(null == p){
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
            p->State = traverseWord(p, c, HelloFinished, null);
            break;
        case HelloFinished:
        case HelloInvalidState:
            break;
    }
    return HelloInvalidState;
}

bool HelloParserHasFailed(HelloParserState state){
    return  state == HelloInvalidState ? true:false;
}

ssize_t HelloParserConsume(HelloParser *p, byte *c, ssize_t length) {
    LogInfo("AuthParser consuming %d bytes",length);
    if (null == p)
    {
        LogError(false,"Cannot consume if HelloParser is NULL");
        return 0;
    }

    if (null == c){
        LogError(false,"HelloParser cannot consume NULL array");
        return 0;
    }

    for (int i = 0; i < length; ++i) {
        HelloParserState state = HelloParserFeed(p,c[i]);
        if (HelloParserHasFinished(state))
            return i+1;
    }
    return length;
}
bool HelloParserHasFinished(HelloParserState state) {
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
