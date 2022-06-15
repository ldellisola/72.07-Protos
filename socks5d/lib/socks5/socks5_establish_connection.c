//
// Created by Lucas Dell'Isola on 14/06/2022.
//

#include <errno.h>
#include "socks5/socks5_establish_connection.h"
#include "socks5/socks5_connection.h"

#define ATTACHMENT(key) ( (Socks5Connection*)((SelectorKey*)(key))->Data)

void EstablishConnectionInit(unsigned int state, void *data) {
//    Socks5Connection * connection = ATTACHMENT(data);
//    EstablishConnectionData * d = &connection->Data.EstablishConnection;
//    d->ReadBuffer = &connection->ReadBuffer;
}

void EstablishConnectionClose(unsigned int state, void *data) {
//    Socks5Connection * connection = ATTACHMENT(data);
//    EstablishConnectionData * d = &connection->Data.EstablishConnection;
//    BufferReset(d->ReadBuffer);
}

unsigned EstablishConnectionRun(void *data) {
    Socks5Connection *connection = ATTACHMENT(data);
    RequestData *d = &connection->Data.Request;

    int isReady = IsTcpConnectionReady(connection->RemoteTcpConnection);

    if (isReady) {
        d->Command = SOCKS5_REPLY_SUCCEEDED;
        return CS_REQUEST_WRITE;
    }

    switch (errno) {
        case EISCONN:
        case EINPROGRESS:
        case EALREADY:
            return CS_ESTABLISH_CONNECTION;
        case EOPNOTSUPP:
            d->Command = SOCKS5_REPLY_CONNECTION_NOT_ALLOWED;
            break;
        case ENETUNREACH:
            d->Command = SOCKS5_REPLY_UNREACHABLE_NETWORK;
            break;
        case EHOSTUNREACH:
            d->Command = SOCKS5_REPLY_UNREACHABLE_HOST;
            break;
        case ECONNREFUSED:
            d->Command = SOCKS5_REPLY_CONNECTION_REFUSED;
            break;
        case ETIMEDOUT:
            d->Command = SOCKS5_REPLY_TTL_EXPIRED;
            break;
        default:
            d->Command = SOCKS5_REPLY_GENERAL_FAILURE;
            break;
    }

    if (SELECTOR_STATUS_SUCCESS !=
        SelectorSetInterest(((SelectorKey *) data)->Selector, connection->ClientTcpConnection->FileDescriptor,
                            SELECTOR_OP_WRITE)) {
        return CS_ERROR;
    }

    if (SELECTOR_STATUS_SUCCESS != SelectorSetInterestKey(data, SELECTOR_OP_NOOP)) {
        return CS_ERROR;
    }

    return CS_REQUEST_WRITE;
}
