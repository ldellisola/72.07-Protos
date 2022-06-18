

#include <memory.h>
#include "utils/logger.h"
#include "parsers/socks5/request_parser.h"

RequestParser RequestParserInit() {
    LogInfo("Creating RequestParser...");
    RequestParser parser;

    RequestParserReset(&parser);

    LogInfo("RequestParser created!");
    return parser;
}

RequestParserState RequestParserFeed(RequestParser *p, byte c) {
    LogInfo("Feeding %d to RequestParser", c);
    if (null == p) {
        LogError(false, "Cannot feed RequestParser if is NULL");
        return RequestInvalidState;
    }

    switch (p->State) {
        case RequestVersion:
            p->State = 0x05 == c ? RequestCMD : RequestErrorUnsupportedVersion;
            LogInfo("RequestParser socks5 protocol version: %d", c);
            break;
        case RequestCMD:
            LogInfo("RequestParser detected command %d", c);
            p->CMD = c;
            p->State = RequestRSV;
            break;
        case RequestRSV:
            LogInfo("RequestParser reserved byte %d", c);
            p->State = 0 == c ? RequestAType : RequestInvalidState;
            break;
        case RequestAType:
            LogInfo("RequestParser address type %d", c);
            p->AType = c;

            switch (p->AType) {
                case SOCKS5_ADDRESS_TYPE_IPV4:
                    p->State = RequestDestAddrIPV4;
                    p->AddressLength = 4;
                    break;
                case SOCKS5_ADDRESS_TYPE_IPV6:
                    p->State = RequestDestAddrIPV6;
                    p->AddressLength = 16;
                    break;
                case SOCKS5_ADDRESS_TYPE_FQDN:
                    p->State = RequestDestAddrFQDN;
                    p->AddressLength = 0;
                    break;
                default:
                    p->State = RequestInvalidState;
                    break;
            }
            break;
        case RequestDestAddrFQDN:

            if (0 == p->AddressLength) {
                p->AddressLength = c;
                if (p->AddressLength <= 0)
                    p->State = RequestInvalidState;
                break;
            }

        case RequestDestAddrIPV4:
        case RequestDestAddrIPV6:

            p->DestAddress[p->AddressPosition++] = c;

            if (p->AddressLength == p->AddressPosition) {
//                LogInfo("RequestParser Address %s", p->DestAddress);
                p->State = RequestDestPortFirstByte;
            }
            break;
        case RequestDestPortFirstByte:
            LogInfo("RequestParser port first byte %d", c);
            p->DestPort[0] = c;
            p->State = RequestDestPortSecondByte;
            break;
        case RequestDestPortSecondByte:
            LogInfo("RequestParser port second byte %d", c);
            p->DestPort[1] = c;
            LogInfo("RequestParser complete port %d", GetPortNumberFromNetworkOrder(p->DestPort));
            p->State = RequestDone;
            break;
        case RequestDone:
        case RequestErrorUnsupportedVersion:
        case RequestInvalidState:
            break;
        default:
            LogError(false, "request invalid state");
            p->State = RequestInvalidState;
            break;
    }
    return p->State;
}

void RequestParserReset(RequestParser *p) {
    LogInfo("Resetting RequestParser...");
    if (null == p) {
        LogError(false, "Cannot reset NULL RequestParser");
        return;
    }

    p->State = RequestVersion;
    p->AddressLength = 0;
    memset(p->DestAddress, 0, 256);
    memset(p->DestPort, 0, 2);
    p->AType = 0;
    p->AddressPosition = 0;
    p->CMD = 0;

    LogInfo("AuthParser reset!");
}

size_t RequestParserConsume(RequestParser *p, byte *c, size_t length) {
    LogInfo("RequestParser consuming %d bytes", length);
    if (null == p) {
        LogError(false, "Cannot consume if RequestParser is NULL");
        return 0;
    }

    if (null == c) {
        LogError(false, "RequestParser cannot consume NULL array");
        return 0;
    }

    for (size_t i = 0; i < length; ++i) {
        RequestParserState state = RequestParserFeed(p, c[i]);
        if (RequestParserHasFinished(state))
            return i + 1;
    }
    return length;
}

bool RequestParserFailed(RequestParserState state) {
    switch (state) {
        case RequestErrorUnsupportedVersion:
        case RequestInvalidState:
            return true;
        default:
            return false;
    }
}

bool RequestParserHasFinished(RequestParserState state) {
    switch (state) {
        case RequestErrorUnsupportedVersion:
        case RequestInvalidState:
        case RequestDone:
            return true;
        default:
            return false;
    }
}
