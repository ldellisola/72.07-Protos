

#include <monetary.h>
#include <memory.h>
#include "parsers/socks5/hello_parser.h"
#include "utils/logger.h"
#include "utils/utils.h"

HelloParser HelloParserInit() {
    LogInfo("Creating HelloParser...");
    HelloParser parser = {
            .State = HelloVersion,
    };
    memset(parser.Methods, 0, 255);

    return parser;
}

HelloParserState HelloParserFeed(HelloParser *p, byte c) {
    LogInfo("Feeding %d to HelloParser", c);

    if (null == p) {
        LogError(false, "Cannot feed HelloParser if is NULL");
        return HelloInvalidState;
    }

    switch (p->State) {
        case HelloVersion:
            LogInfo("HelloParser socks5 protocol version: %d", c);
            p->State = 0x05 == c ? HelloNMethods : HelloErrorUnsupportedVersion;
            break;
        case HelloNMethods:
            LogInfo("HelloParser nmethods: %d", c);
            p->NMethods = c;
            p->State = HelloMethods;

            if (p->NMethods <= 0)
                p->State = HelloInvalidState;

            break;
        case HelloMethods:
            p->Methods[p->CurrentMethod++] = c;
            LogInfo("HelloParser detected authentication method %d", c);
            if (p->CurrentMethod == p->NMethods) {
                LogInfo("HelloParser no more authentication methods");
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
    LogInfo("HelloParser consuming %d bytes", length);
    if (null == p) {
        LogError(false, "Cannot consume if HelloParser is NULL");
        return 0;
    }

    if (null == c) {
        LogError(false, "HelloParser cannot consume NULL array");
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
    LogInfo("Resetting HelloParser...");
    if (null == p) {
        LogError(false, "Cannot reset NULL HelloParser");
        return;
    }

    p->State = HelloVersion;
    p->NMethods = 0;
    p->CurrentMethod = 0;
    memset(p->Methods, 0, 255);

    LogInfo("AuthParser value reset!");
}
