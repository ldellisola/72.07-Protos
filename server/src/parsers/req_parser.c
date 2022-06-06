

#include "../../headers/logger.h"
#include "../../headers/parsers/req_parser.h"

RequestParser *RequestParserInit() {
    LogInfo("Creating RequestParser...");
    RequestParser * ptr = calloc(1,sizeof(RequestParser));

    if (null == ptr) {
        LogError(false, "Cannot allocate RequestParser");
        return ptr;
    }

    ptr->State = RequestVersion;
    LogInfo("RequestParser created!");
    return ptr;}

bool RequestParserFeed(RequestParser *p, byte c) {
    LogInfo("Feeding %d to RequestParser",c);
    if (null == p)
    {
        LogError(false,"Cannot feed RequestParser if is NULL");
        return true;
    }

    switch (p->State)
    {
        case RequestVersion:
            p->State = 0x05 == c ? RequestCMD : RequestErrorUnsupportedVersion;
            LogInfo("RequestParser socks5 protocol version: %d",c);
            break;
        case RequestCMD:
            LogInfo("RequestParser detected command %d",c);
            p->CMD = c;
            p->CMD = RequestRSV;
            break;
        case RequestRSV:
            LogInfo("RequestParser reserved byte %d",c);
            p->State = RequestAType;
            break;
        case RequestAType:
            LogInfo("RequestParser address type %d",c);
            p->AType = c;

            if (ATYP_IPV4 == p->AType) {
                p->State = RequestDestAddrIPV4;
                p->AddressLength = 4;
            }

            if (ATYP_IPV6 == p->AType) {
                p->State = RequestDestAddrIPV6;
                p->AddressLength = 16;
            }

            if (ATYP_DOMAINNAME == p->AType)
                p->State = RequestDestAddrFQDN;

            break;
        case RequestDestAddrFQDN:
            if (null == p->DestAddress && 0 == p->AddressLength){
                p->AddressLength = c;
                break;
            }
        case RequestDestAddrIPV4:
        case RequestDestAddrIPV6:
            if (null == p->DestAddress)
                p->DestAddress = calloc(p->AddressLength + 1, sizeof(uint8_t));

            p->DestAddress[p->AddressPosition++] = c;

            if(p->AddressLength == p->AddressPosition) {
                LogInfo("RequestParser Address %s",p->DestAddress);
                p->State = RequestDestPortFirstByte;
            }
            break;
        case RequestDestPortFirstByte:
            LogInfo("RequestParser port first byte %x",c);
            p->DestPort.First = c;
            p->State = RequestDestPortSecondByte;
            break;
        case RequestDestPortSecondByte:
            LogInfo("RequestParser port second byte %x",c);
            p->DestPort.Second = c;
            p->State = RequestDone;
            break;
        case RequestDone:
        case RequestErrorUnsupportedVersion:
            return true;
        default:
            LogError(false, "request invalid state");
            p->State = RequestInvalidState;
            break;
    }
    return false;
}

void RequestParserDestroy(RequestParser *p) {
    LogInfo("Disposing RequestParser...");
    if (null == p) {
        LogError(false, "Cannot destroy NULL RequestParser");
        return;
    }

    if (null != p->DestAddress)
        free(p->DestAddress);

    free(p);
    LogInfo("AuthParser disposed!");
}

bool RequestParserConsume(RequestParser *p, byte *c, int length) {
    LogInfo("RequestParser consuming %d bytes",length);
    if (null == p)
    {
        LogError(false,"Cannot consume if RequestParser is NULL");
        return true;
    }

    if (null == c){
        LogError(false,"RequestParser cannot consume NULL array");
        return true;
    }

    for (int i = 0; i < length; ++i) {
        if (RequestParserFeed(p,c[i]))
            return true;
    }
    return false;
}

bool RequestParserFinished(RequestParser *p, bool* hasError){
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
        case RequestDone:
            *hasError = false;
            return true;
        case RequestErrorUnsupportedVersion:
        case RequestInvalidState:
            *hasError = true;
            return true;
        default:
            *hasError = false;
            return false;
    }
}
