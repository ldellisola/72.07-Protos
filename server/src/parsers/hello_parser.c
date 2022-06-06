

#include "../../headers/parsers/hello_parser.h"
#include "../../headers/logger.h"
#include "../../headers/utils.h"

HelloParser * HelloParserInit()
{
    LogInfo("Creating HelloParser...");
    HelloParser * p = calloc(1,sizeof(HelloParser));

    if (null == p) {
        LogError(false, "Cannot allocate space for HelloParser");
        return null;
    }

    return p;
}

bool HelloParserFeed(HelloParser *p, uint8_t b)
{
    LogInfo("Feeding %d to HelloParser",b);

    if (null == p)
    {
        LogError(false,"Cannot feed HelloParser if is NULL");
        return true;
    }

    switch (p->State)
    {
        case HelloVersion:
            p->State = 0x05 == b ? HelloNMethods : HelloErrorUnsupportedVersion;
            LogInfo("HelloParser socks5 protocol version: %d",b);
            break;
        case HelloNMethods:
            LogInfo("HelloParser nmethods: %d",b);
            p->NMethods = b;
            p->State = HelloMethods;

            if (p->NMethods <= 0)
                p->State = HelloDone;
            else
                p->Methods = calloc(p->NMethods,sizeof(int));

            break;
        case HelloMethods:
            p->Methods[p->RemainingMethods++] = b;
            LogInfo("HelloParser detected authentication method %d",b);
            if (p->RemainingMethods == p->NMethods) {
                LogInfo("HelloParser no more authentication methods");
                p->State = HelloDone;
            }
            break;
        case HelloDone:
        case HelloErrorUnsupportedVersion:
            return true;
            break;
        default:
            p->State = HelloInvalidState;
            break;
    }
    return false;
}

bool HelloParserFinished(HelloParser * p, bool *hasError)
{
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
    bool ret = false;
    switch (p->State)
    {
        case HelloErrorUnsupportedVersion:
            *hasError = true;
            ret = true;
            break;
        case HelloDone:
            ret = true;
            break;
        default:
            ret = false;
            break;
    }
    return ret;
}

bool HelloParserConsume(HelloParser * p, byte * c, int length){
    LogInfo("HelloParser consuming %d bytes",length);
    if (null == p)
    {
        LogError(false,"Cannot consume if HelloParser is NULL");
        return true;
    }

    if (null == c){
        LogError(false,"HelloParser cannot consume NULL array");
        return true;
    }

    for (int i = 0; i < length; ++i) {
        if (HelloParserFeed(p,c[i]))
            return true;
    }
    return false;
}

void HelloParserDestroy(HelloParser *p) {
    LogInfo("Disposing HelloParser...");
    if (null == p) {
        LogError(false, "Cannot destroy NULL HelloParser");
        return;
    }

    if (null != p->Methods)
        free(p->Methods);

    free(p);
    LogInfo("AuthParser disposed!");
}
