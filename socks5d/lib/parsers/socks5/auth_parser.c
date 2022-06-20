//
// Created by Lucas Dell'Isola on 05/06/2022.
//

#include <memory.h>
#include "parsers/socks5/auth_parser.h"
#include "utils/logger.h"



void AuthParserReset(AuthParser *p) {
    Debug("Resetting AuthParser...");
    if (null == p) {
        Warning( "Cannot reset NULL AuthParser");
        return;
    }

    p->State = AuthVersion;
    p->PLen = 0;
    p->ULen = 0;
    p->PasswdPosition = 0;
    p->UNamePosition = 0;


    memset(p->UName, 0, 256);
    memset(p->Passwd, 0, 256);

    Debug("AuthParser reset!");
}

AuthParserState AuthParserFeed(AuthParser *p, byte c) {
    LogDebug("Feeding %d to AuthParser", c);
    if (null == p) {
        Warning( "Cannot feed AuthParser if is NULL");
        return AuthInvalidState;
    }

    switch (p->State) {
        case AuthVersion:
            p->State = 0x01 == c ? AuthULen : AuthInvalidProtocol;
            LogDebug("AuthParser socks5 protocol version: %d", c);
            break;
        case AuthULen:
            LogDebug("AuthParser username length: %d", c);
            p->ULen = c;
            p->State = 0 == p->ULen ? AuthInvalidState : AuthUName;
            break;
        case AuthUName:

            p->UName[p->UNamePosition++] = c;

            if (p->UNamePosition == p->ULen) {
                LogDebug("AuthParser username: %s", p->UName);
                p->State = AuthPLen;
            }
            break;
        case AuthPLen:
            LogDebug("AuthParser password length: %d", c);
            p->PLen = c;
            p->State = 0 == p->PLen ? AuthInvalidState : AuthPasswd;
            break;
        case AuthPasswd:

            p->Passwd[p->PasswdPosition++] = c;

            if (p->PasswdPosition == p->PLen) {
                LogDebug("AuthParser password: %s", p->Passwd);
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

bool AuthParserHasFailed(AuthParserState state) {

    switch (state) {
        case AuthInvalidState:
        case AuthInvalidProtocol:
            return true;
        default:
            return false;
    }
}

size_t AuthParserConsume(AuthParser *p, byte *c, size_t length) {
    LogDebug("AuthParser consuming %d bytes", length);
    if (null == p) {
        Warning("Cannot consume if AuthParser is NULL");
        return 0;
    }

    if (null == c) {
        Warning( "AuthParser cannot consume NULL array");
        return 0;
    }

    for (size_t i = 0; i < length; ++i) {
        AuthParserState state = AuthParserFeed(p, c[i]);
        if (AuthParserHasFinished(state))
            return i + 1;
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















