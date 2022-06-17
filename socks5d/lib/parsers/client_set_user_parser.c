//
// Created by tluci on 17/6/2022.
//

#include "parsers/client_set_user_parser.h"
#include "utils/logger.h"

ClientSetUserParserState traverseWordSetUser(ClientSetUserParser *p, byte c, ClientSetUserParserState nextState, char *nextWord) {
//    LogError(false, "char c = %c", c);
    if(strlen(p->Word) == p->Index){
        if(c == '|'){
            p->Word = nextWord;
            p->Index = 0;
            return nextState;
        }
        LogError(false, "The word has finished and character given isnt a terminating character");
        return UserInvalidState;

    }

    if(c == p->Word[p->Index]){
        p->Index++;
        return p->State;
    }
    LogError(false, "Waiting for a \"%c\" and got a \"%c\" in the word \" %s \"", p->Word[p->Index], c, p->Word);

    return UserInvalidState;

}

void ClientSetUserParserReset(ClientSetUserParser *p) {
    LogInfo("Resetting SetUserParser...");
    if (null == p) {
        LogError(false, "Cannot reset NULL SetUserParser");
        return;
    }

    p->State = UserSet;
    p->Index = 0;

    memset(p->UName, 0, MAXLONG);
    memset(p->Passwd, 0, MAXLONG);
    p->User[0] = 'U';
    p->User[1] = 'S';
    p->User[2] = 'E';
    p->User[3] = 'R';
    p->User[4] = 0;

    p->Set[0] = 'S';
    p->Set[1] = 'E';
    p->Set[2] = 'T';
    p->Set[3] = 0;

    p->Word = p->Set;
    p->PrevState = UserSet;
    LogInfo("SetUserParser reset!");
}

ClientSetUserParserState ClientSetUserParserFeed(ClientSetUserParser *p, byte c) {
    LogInfo("Feeding %d to ClientSetUserParser", c);
    if (null == p) {
        LogError(false, "Cannot feed SetUserParser if is NULL");
        return UserInvalidState;
    }

    switch (p->State) {
        case UserSet:
            p->State = traverseWordSetUser(p, c, User, p->User);
            break;
        case User:
            p->State = traverseWordSetUser(p, c, UserName, p->UName);
            break;
        case UserName:
            if(c == '|'){
                if(p->Index == 0){
                    LogError(false, "Username must be at least 1 character long");
                    p->State = UserInvalidState;
                    break;
                }
                p->State = UserPassword;
                p->Word = p->Passwd;
                p->Index = 0;
                break;
            }
            if(c == '\r'){
                p->PrevState = UserName;
                p->State = UserCRLF;
                break;
            }
            if(p->Index == MAXLONG+1){
                LogError(false, "Username can be up to 255 characters long");
                p->State = UserInvalidState;
                break;
            }
            p->Word[p->Index] = (char )c;
            p->Index++;
            break;
        case UserPassword:
            if(c == '|'){
                p->State = UserInvalidState;
                break;
            }
            if(c == '\r'){
                p->PrevState = UserPassword;
                p->State = UserCRLF;
                break;
            }
            if(p->Index == MAXLONG+1){
                LogError(false, "Password can be up to 50 characters long");
                p->State = UserInvalidState;
                break;
            }
            p->Word[p->Index] = (char )c;
            p->Index++;
            break;
        case UserCRLF:
            if(c =='\n'){
                if(p->PrevState == UserPassword){
                    if(p->Index == 0){
                        LogError(false, "Password must be at least 1 character long");
                        p->State = UserInvalidState;
                        break;
                    }
                    p->State = UserFinished;
                    break;
                }
                LogError(false, "More Arguments needed");
                p->State = UserInvalidState;
                break;
            }
            if(p->Index == MAXLONG+1){
                LogError(false, "Username and Password can be up to 50 characters long");
                p->State = UserInvalidState;
                break;
            }
            if(c =='\r'){
                p->Word[p->Index] = '\r';
                p->Index++;
                break;
            }
            p->Word[p->Index] = '\r';
            p->Index++;
            p->Word[p->Index] = (char)c;
            p->Index++;
            p->State = p->PrevState;
            break;
        case UserFinished:
        case UserInvalidState:
            break;
    }
    return p->State;
}

bool ClientSetUserParserHasFailed(ClientSetUserParserState state) {
    return state == UserInvalidState ? true : false;
}

size_t ClientSetUserParserConsume(ClientSetUserParser *p, byte *c, size_t length) {
    LogInfo("SetUserParser consuming %d bytes", length);
    if (null == p) {
        LogError(false, "Cannot consume if SetUserParser is NULL");
        return 0;
    }

    if (null == c) {
        LogError(false, "SetUserParser cannot consume NULL array");
        return 0;
    }

    for (size_t i = 0; i < length; ++i) {
        ClientSetUserParserState state = ClientSetUserParserFeed(p, c[i]);
        if (ClientSetUserParserHasFinished(state))
            return i + 1;
    }
    return length;
}

bool ClientSetUserParserHasFinished(ClientSetUserParserState state) {
    switch (state) {
        default:
        case UserSet:
        case User:
        case UserName:
        case UserPassword:
        case UserCRLF:
            return false;
        case UserInvalidState:
        case UserFinished:
            return true;
    }
}
