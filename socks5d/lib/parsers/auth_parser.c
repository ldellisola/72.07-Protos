//
// Created by Lucas Dell'Isola on 05/06/2022.
//

#include <memory.h>
#include <monetary.h>
#include "parsers/auth_parser.h"
#include "utils/logger.h"

//AuthParser AuthParserInit() {
//    LogInfo("Creating AuthParser...");
//
//    AuthParser parser;
//    AuthParserReset(&parser);
//
//    LogInfo("AuthParser created!");
//    return parser;
//}

void AuthParserReset(AuthParser *p) {
    LogInfo("Resetting AuthParser...");
    if (null == p) {
        LogError(false, "Cannot reset NULL AuthParser");
        return;
    }

    p->State = AuthVersion;
    p->PLen = 0;
    p->ULen = 0;
    p->PasswdPosition = 0;
    p->UNamePosition = 0;


    memset(p->UName,0,256);
    memset(p->Passwd,0,256);

    LogInfo("AuthParser reset!");
}

AuthParserState AuthParserFeed(AuthParser *p, byte c) {
    LogInfo("Feeding %d to AuthParser",c);
    if (null == p)
    {
        LogError(false,"Cannot feed AuthParser if is NULL");
        return AuthInvalidState;
    }

    switch (p->State) {
        case AuthVersion:
            p->State = 0x05 == c ? AuthULen : AuthInvalidProtocol;
            LogInfo("AuthParser socks5 protocol version: %d",c);
            break;
        case AuthULen:
            LogInfo("AuthParser username length: %d",c);
            p->ULen = c;
            p->State = 0 == p->ULen ? AuthInvalidState : AuthUName;
            break;
        case AuthUName:

            p->UName[p->UNamePosition++] = c;

            if (p->UNamePosition == p->ULen) {
                LogInfo("AuthParser username: %Selector",p->UName);
                p->State = AuthPLen;
            }
            break;
        case AuthPLen:
            LogInfo("AuthParser password length: %d",c);
            p->PLen = c;
            p->State =  0 == p->PLen ? AuthInvalidState : AuthPasswd;
            break;
        case AuthPasswd:

            p->Passwd[p->PasswdPosition++] = c;

            if (p->PasswdPosition == p->PLen) {
                LogInfo("AuthParser password: %Selector",p->Passwd);
                p->State = AuthFinished;
            }
            break;
        case AuthInvalidProtocol:
        case AuthFinished:
        case AuthInvalidState:
            break;
    }

    return p->State;
}

bool AuthParserHasFailed(AuthParserState state){

    switch (state) {
        case AuthInvalidState:
        case AuthInvalidProtocol:
            return true;
        default:
            return false;
    }
}

ssize_t AuthParserConsume(AuthParser *p, byte *c, ssize_t length) {
    LogInfo("AuthParser consuming %d bytes",length);
    if (null == p)
    {
        LogError(false,"Cannot consume if AuthParser is NULL");
        return 0;
    }

    if (null == c){
        LogError(false,"AuthParser cannot consume NULL array");
        return 0;
    }

    for (int i = 0; i < length; ++i) {
        AuthParserState state = AuthParserFeed(p,c[i]);
        if (AuthParserHasFinished(state))
            return i+1;
    }
    return length;
}

bool AuthParserHasFinished(AuthParserState state) {
    switch (state) {
        default:
        case AuthVersion:
        case AuthULen:
        case AuthUName:
        case AuthPLen:
        case AuthPasswd:
            return false;
        case AuthInvalidProtocol:
        case AuthInvalidState:
        case AuthFinished:
            return true;
    }
}















