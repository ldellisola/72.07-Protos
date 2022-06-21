//
// Created by tluci on 17/6/2022.
//

#include "parsers/lulu/client_del_user_parser.h"
#include "utils/logger.h"

ClientDelUserParserState traverseWordDel(ClientDelUserParser *p, byte c, ClientDelUserParserState nextState, char *nextWord) {
//    Error( "char c = %c", c);
    if(strlen(p->Word) == p->Index){
        if(c == '|'){
            p->Word = nextWord;
            p->Index = 0;
            return nextState;
        }
        Debug("Im in the last letter of the word and there is no pipe");
        return DelInvalidState;
    }
    if (c == p->Word[p->Index]) {
        p->Index++;
        return p->State;
    }
    LogDebug( "wrong character for word, was waiting for %c and got %c", p->Word[p->Index], c);
    return DelInvalidState;

}

void ClientDelUserParserReset(ClientDelUserParser *p) {
    Debug("Resetting DelParser...");
    if (null == p) {
        Debug("Cannot reset NULL DelParser");
        return;
    }

    p->State = Del;
    p->Index = 0;

    memset(p->UName, 0, MAXLONG);
    p->Del[0] = 'D';
    p->Del[1] = 'E';
    p->Del[2] = 'L';
    p->Del[3] = 0;
    p->User[0] = 'U';
    p->User[1] = 'S';
    p->User[2] = 'E';
    p->User[3] = 'R';
    p->User[4] = 0;

    p->Word = p->Del;
    Debug("DelUserParser reset!");
}

ClientDelUserParserState ClientDelUserParserFeed(ClientDelUserParser *p, byte c) {
    LogDebug("Feeding %d to ClientDelUserParser", c);
//    Error( "char= %c", c);
    if (null == p) {
        Debug("Cannot feed DelUserParser if is NULL");
        return DelInvalidState;
    }

    switch (p->State) {
        case Del:
            p->State = traverseWordDel(p, c, DelUser, p->User);
            break;
        case DelUser:
            p->State = traverseWordDel(p, c, DelUserName, p->UName);
            break;
        case DelUserName:
            if(c == '|'){
                Debug( "Too many arguments");
                p->State = DelInvalidState;
                break;
            }
            if(c == '\r'){
                p->State = DelCRLF;
                break;
            }
            if(p->Index == MAXLONG+1){
                Debug("Username can have max 255 characters");
                p->State = DelInvalidState;
                break;
            }
            p->Word[p->Index] = (char)c;
            p->Index++;
            break;
        case DelCRLF:
            if(c == '\n'){
                if(p->Index == 0){
                    p->State = DelInvalidState;
                    Debug("Username has to be at least 1 character long");
                    break;
                }
                p->Word[p->Index] = '\0';
                p->State = DelFinished;
                break;
            }
            if(c == '|'){
                Debug("Too many arguments");
                p->State = DelInvalidState;
                break;
            }
            if(c == '\r'){
                if(p->Index == MAXLONG+1){
                    Debug("Username can have max 255 characters");
                    p->State = DelInvalidState;
                    break;
                }
                p->Word[p->Index] = '\r';
                p->Index++;
                break;
            }

            if(p->Index == MAXLONG){
                Debug( "Username and password can have max 255 characters");
                p->State = DelInvalidState;
                break;
            }

            p->Word[p->Index] = '\r';
            p->Index++;
            p->Word[p->Index] = (char)c;
            p->Index++;
            p->State = DelUserName;
            break;
        case DelFinished:
        case DelInvalidState:
            break;
    }
    return p->State;
}

bool ClientDelUserParserHasFailed(ClientDelUserParserState state) {
    return state == DelInvalidState ? true : false;
}

size_t ClientDelUserParserConsume(ClientDelUserParser *p, byte *c, size_t length) {
    LogDebug("DelUserParser consuming %d bytes", length);
    if (null == p) {
        Debug("Cannot consume if DelUserParser is NULL");
        return 0;
    }

    if (null == c) {
        Debug("DelUserParser cannot consume NULL array");
        return 0;
    }

    for (size_t i = 0; i < length; ++i) {
        ClientDelUserParserState state = ClientDelUserParserFeed(p, c[i]);
        if (ClientDelUserParserHasFinished(state))
            return i + 1;
    }
    return length;
}

bool ClientDelUserParserHasFinished(ClientDelUserParserState state) {
    switch (state) {
        default:
        case Del:
        case DelUser:
        case DelUserName:
        case DelCRLF:
            return false;
        case DelInvalidState:
        case DelFinished:
            return true;
    }
}
