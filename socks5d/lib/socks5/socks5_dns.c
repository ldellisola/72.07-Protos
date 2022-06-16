//
// Created by Lucas Dell'Isola on 16/06/2022.
//

#include "socks5/socks5_dns.h"
#include "socks5/socks5_connection.h"

#define ATTACHMENT(key) ( (Socks5Connection*)((SelectorKey*)(key))->Data)


unsigned DnsRead(void *data) {
    SelectorKey * key = (SelectorKey*) data;
    Socks5Connection * connection = ATTACHMENT(data);
    RequestData * d = &connection->Data.Request;

    if (null == d->RemoteAddress) {
        d->Command = SOCKS5_REPLY_GENERAL_FAILURE;
        int selectorResult = SelectorSetInterest(key->Selector, connection->ClientTcpConnection->FileDescriptor,SELECTOR_OP_WRITE);
        return SELECTOR_STATUS_SUCCESS == selectorResult ? CS_REQUEST_WRITE : CS_ERROR;
    }


    return 0;
}
