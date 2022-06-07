//
// Created by Lucas Dell'Isola on 05/06/2022.
//

#include "parsers/auth_parser.h"
#include "utils/logger.h"

AuthParser * AuthParserInit() {
    LogInfo("Creating AuthParser...");
    AuthParser * ptr = calloc(1,sizeof(AuthParser));

    if (null == ptr) {
        LogError(false, "Cannot allocate AuthParser");
        return ptr;
    }

    ptr->State = AuthVersion;
    LogInfo("AuthParser created!");
    return ptr;
}

void AuthParserDestroy(AuthParser *p) {
    LogInfo("Disposing AuthParser...");
    if (null == p) {
        LogError(false, "Cannot destroy NULL AuthParser");
        return;
    }

    if (null != p->UName)
        free(p->UName);

    if (null != p->Passwd)
        free(p->Passwd);

    free(p);
    LogInfo("AuthParser disposed!");
}

bool AuthParserFeed(AuthParser *p, byte c) {
    LogInfo("Feeding %d to AuthParser",c);
    if (null == p)
    {
        LogError(false,"Cannot feed AuthParser if is NULL");
        return true;
    }

    switch (p->State) {
        case AuthVersion:
            p->State = 0x05 == c ? AuthULen : AuthInvalidProtocol;
            LogInfo("AuthParser socks5 protocol version: %d",c);
            break;
        case AuthULen:
            LogInfo("AuthParser username length: %d",c);
            p->ULen = c;
            p->State = AuthUName;
            break;
        case AuthUName:
            if (null == p->UName)
                p->UName = calloc(p->ULen+1,sizeof(char));

            p->UName[p->UNamePosition++] = c;

            if (p->UNamePosition == p->ULen) {
                LogInfo("AuthParser username: %s",p->UName);
                p->State = AuthPLen;
            }
            break;
        case AuthPLen:
            LogInfo("AuthParser password length: %d",c);
            p->PLen = c;
            p->State = AuthPasswd;
            break;
        case AuthPasswd:
            if (null == p->Passwd)
                p->Passwd = calloc(p->PLen +1,sizeof(char));

            p->Passwd[p->PasswdPosition++] = c;

            if (p->PasswdPosition == p->PLen) {
                LogInfo("AuthParser password: %s",p->Passwd);
                p->State = AuthFinished;
            }
            break;
        case AuthInvalidProtocol:
        case AuthFinished:
            return true;
    }

    return false;
}

bool AuthParserFinished(AuthParser *p,bool*hasError){

    if (null == hasError){
        LogError(false,"HasError reference cannot be NULL");
        return false;
    }

    if (null == p)
    {
        LogError(false,"Cannot feed AuthParser if is NULL");
        *hasError = true;
        return true;
    }

    switch (p->State) {
        case AuthFinished:
            *hasError = false;
            return true;
        case AuthInvalidProtocol:
            *hasError = true;
            return true;
        default:
            *hasError = false;
            return false;
    }
}

bool AuthParserConsume(AuthParser *p, byte *c, int length) {
    LogInfo("AuthParser consuming %d bytes",length);
    if (null == p)
    {
        LogError(false,"Cannot consume if AuthParser is NULL");
        return true;
    }

    if (null == c){
        LogError(false,"AuthParser cannot consume NULL array");
        return true;
    }

    for (int i = 0; i < length; ++i) {
        if (AuthParserFeed(p,c[i]))
            return true;
    }
    return false;
}















