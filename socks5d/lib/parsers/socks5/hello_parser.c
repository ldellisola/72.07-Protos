

#include <memory.h>
#include "parsers/socks5/hello_parser.h"
#include "utils/logger.h"
#include "utils/utils.h"


HelloParserState HelloParserFeed(HelloParser *p, byte c) {
    LogDebug("Feeding %d to HelloParser", c);

    if (null == p) {
        LogDebug(false, "Cannot feed HelloParser if is NULL");
        return HelloInvalidState;
    }

    switch (p->State) {
        case HelloVersion:
            LogDebug("HelloParser socks5 protocol version: %d", c);
            p->State = 0x05 == c ? HelloNMethods : HelloErrorUnsupportedVersion;
            break;
        case HelloNMethods:
            LogDebug("HelloParser nmethods: %d", c);
            p->NMethods = c;
            p->State = HelloMethods;

            if (p->NMethods <= 0)
                p->State = HelloInvalidState;

            break;
        case HelloMethods:
            p->Methods[p->CurrentMethod++] = c;
            LogDebug("HelloParser detected authentication method %d", c);
            if (p->CurrentMethod == p->NMethods) {
                Debug("HelloParser no more authentication methods");
                p->State = HelloDone;
            }
            break;
        case HelloDone:
        case HelloErrorUnsupportedVersion:
            break;
        default:
            p->State = HelloInvalidState;
            break;
    }
    return p->State;
}

bool HelloParserHasFailed(HelloParserState state) {

    switch (state) {
        case HelloInvalidState:
        case HelloErrorUnsupportedVersion:
            return true;
        default:
            return false;
    }
}

bool HelloParserHasFinished(HelloParserState state) {
    switch (state) {
        case HelloDone:
        case HelloInvalidState:
        case HelloErrorUnsupportedVersion:
            return true;
        default:
            return false;
    }
}

size_t HelloParserConsume(HelloParser *p, byte *c, size_t length) {
    LogDebug("HelloParser consuming %d bytes", length);
    if (null == p) {
        Warning("Cannot consume if HelloParser is NULL");
        return 0;
    }

    if (null == c) {
        Warning("HelloParser cannot consume NULL array");
        return 0;
    }

    for (size_t i = 0; i < length; ++i) {
        HelloParserState state = HelloParserFeed(p, c[i]);
        if (HelloParserHasFinished(state))
            return i + 1;
    }
    return length;
}

void HelloParserReset(HelloParser *p) {
    Debug("Resetting HelloParser...");
    if (null == p) {
        Warning( "Cannot reset NULL HelloParser");
        return;
    }

    p->State = HelloVersion;
    p->NMethods = 0;
    p->CurrentMethod = 0;
    memset(p->Methods, 0, 255);

    Debug("HelloParser value reset!");
}
