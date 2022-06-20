//
// Created by Lucas Dell'Isola on 14/06/2022.
//

#include <errno.h>
#include "socks5/fsm_handlers/socks5_establish_connection.h"
#include "socks5/socks5_connection.h"
#include "utils/logger.h"


void EstablishConnectionInit(unsigned int state, void *data) {
    SelectorKey * key = (SelectorKey *) data;
    Socks5Connection *connection = ATTACHMENT(data);
    RequestData *d = &connection->Data.Request;

    // This means that I'm currently connecting to an address
    if (null == d->CurrentRemoteAddress)
        return;


    // If I'm reentering and reaching this stage, it means one of two things:
    // 1. It is the first time I'm here, so this connection will be null
    // 2. It is a reentry after the previous failed attempt. It has to dispose the previously
    //    opened tcp connection
    if (null != connection->RemoteTcpConnection)
        DisposeTcpConnection(connection->RemoteTcpConnection,key->Selector);

    TcpConnection *remoteConnection = null;

    if (AF_INET == d->CurrentRemoteAddress->ai_family) {
        char buffer[INET_ADDRSTRLEN +1];
        GetIPFromAddress((struct sockaddr_storage *) d->CurrentRemoteAddress->ai_addr, buffer, INET_ADDRSTRLEN + 1);
        LogDebug("Attempting to connect to IPv4 %s",buffer);
        remoteConnection = ConnectToIPv4TcpServer(
                d->CurrentRemoteAddress->ai_addr,
                connection->Handler,
                connection
        );
    }

    if (AF_INET6 == d->CurrentRemoteAddress->ai_family) {
        char buffer[INET6_ADDRSTRLEN +1];
        GetIPFromAddress((struct sockaddr_storage *) d->CurrentRemoteAddress->ai_addr, buffer, INET6_ADDRSTRLEN + 1);
        LogDebug("Attempting to connect to IPv6 %s",buffer);
        remoteConnection = ConnectToIPv6TcpServer(
                d->CurrentRemoteAddress->ai_addr,
                connection->Handler,
                connection
        );
    }

    // free address if not dns
    if (null == d->RemoteAddress && null != d->CurrentRemoteAddress){
        Debug("Releasing remote address memory because it was not resolved by DNS");
        free(d->CurrentRemoteAddress->ai_addr);
        free(d->CurrentRemoteAddress);
    }


    if (null == remoteConnection) {
        LogWarningWithReason("Cannot connect to remote address %s",connection->RemoteAddressString);
        d->Command = SOCKS5_REPLY_GENERAL_FAILURE;
        SelectorSetInterestKey(key,SELECTOR_OP_READ | SELECTOR_OP_WRITE);
    }

    connection->RemoteTcpConnection = remoteConnection;
}

void EstablishConnectionClose(unsigned int state, void *data) {

    Socks5Connection * connection = ATTACHMENT(data);
    RequestData * d = &connection->Data.Request;

    // free dns address
    if (null != d->RemoteAddress) {
        Debug("Disposing DNS resolved addresses");
        freeaddrinfo(d->RemoteAddress);
    }
}

unsigned EstablishConnectionRun(void *data) {
    SelectorKey * key = (SelectorKey *) data;
    Socks5Connection *connection = ATTACHMENT(data);
    RequestData *d = &connection->Data.Request;
    unsigned selectorResult = 0;

    if (d->Command != SOCKS5_REPLY_NOT_DECIDED){
        // If there are more addresses to try, I will reset the error and look for the next one
        if (null != d->CurrentRemoteAddress->ai_next){
            d->Command = SOCKS5_REPLY_NOT_DECIDED;
            d->CurrentRemoteAddress = d->CurrentRemoteAddress->ai_next;
            return CS_ESTABLISH_CONNECTION;
        }
        // If I'm here it's because it never connected, so I don't have to dispose the remote connection
        selectorResult = SelectorSetInterest(key->Selector, connection->ClientTcpConnection->FileDescriptor,SELECTOR_OP_WRITE);
        return SELECTOR_STATUS_SUCCESS == selectorResult ? CS_REQUEST_WRITE : CS_ERROR;
    }

    int isReady = IsTcpConnectionReady(connection->RemoteTcpConnection);

    if (isReady) {
        d->Command = SOCKS5_REPLY_SUCCEEDED;
        selectorResult |= SelectorSetInterest(key->Selector, connection->ClientTcpConnection->FileDescriptor,SELECTOR_OP_WRITE);
        selectorResult |= SelectorSetInterest(key->Selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_NOOP);

        return SELECTOR_STATUS_SUCCESS == selectorResult ? CS_REQUEST_WRITE : CS_ERROR;
    }

    switch (errno) {
        case EISCONN:
        case EINPROGRESS:
        case EALREADY:
            d->CurrentRemoteAddress = null;
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

    // If there are more addresses to try, I will reset the error and look for the next one
    if (null != d->CurrentRemoteAddress->ai_next){
        d->Command = SOCKS5_REPLY_NOT_DECIDED;
        d->CurrentRemoteAddress = d->CurrentRemoteAddress->ai_next;
        return CS_ESTABLISH_CONNECTION;
    }

    selectorResult |= SelectorSetInterest(key->Selector, connection->ClientTcpConnection->FileDescriptor,SELECTOR_OP_WRITE);
    selectorResult |= SelectorSetInterest(key->Selector, connection->RemoteTcpConnection->FileDescriptor, SELECTOR_OP_NOOP);

    return SELECTOR_STATUS_SUCCESS == selectorResult ? CS_REQUEST_WRITE : CS_ERROR;

}
