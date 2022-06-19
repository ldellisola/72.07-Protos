//
// Created by Lucas Dell'Isola on 16/06/2022.
//

#include "socks5/fsm_handlers/socks5_dns.h"
#include "socks5/socks5_connection.h"
#include "utils/logger.h"

#define ATTACHMENT(key) ( (Socks5Connection*)((SelectorKey*)(key))->Data)

int CountAddresses(struct addrinfo * address);

unsigned DnsRead(void *data) {
    SelectorKey * key = (SelectorKey*) data;
    Socks5Connection * connection = ATTACHMENT(data);
    RequestData * d = &connection->Data.Request;

    if (null == d->RemoteAddress) {
        LogError("DNS could not resolve FQDN %s",connection->RemoteAddressString);
        d->Command = SOCKS5_REPLY_UNREACHABLE_HOST;
        int selectorResult = SelectorSetInterest(key->Selector, connection->ClientTcpConnection->FileDescriptor,SELECTOR_OP_WRITE);
        return SELECTOR_STATUS_SUCCESS == selectorResult ? CS_REQUEST_WRITE : CS_ERROR;
    }

    LogDebug("DNS resolved FQDN %s to %d addresses",connection->RemoteAddressString, CountAddresses(d->RemoteAddress));

    d->CurrentRemoteAddress = d->RemoteAddress;

    return CS_ESTABLISH_CONNECTION;
}

int CountAddresses(struct addrinfo * address){
    if (null == address)
        return 0;

    return 1 + CountAddresses(address->ai_next);
}

