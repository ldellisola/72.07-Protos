//
// Created by Lucas Dell'Isola on 20/06/2022.
//

#include <errno.h>
#include "lulu_client/fsm/client_wait_connection.h"
#include "lulu_client/client_data.h"

#define ATTACHMENT(key) ( (ClientData *)((SelectorKey*)(key))->Data)

unsigned LuluClientConnect(void *data) {
    ClientData *  d = ATTACHMENT(data);

    int isReady = IsTcpConnectionReady(d->Remote);

    if (isReady) {
        int selectorResult = SelectorSetInterestKey(data,SELECTOR_OP_WRITE);
        return SELECTOR_STATUS_SUCCESS == selectorResult ? CS_WRITE_AUTH : CS_ERROR;
    }

    switch (errno) {
        case EISCONN:
        case EINPROGRESS:
        case EALREADY:
            return CS_WAITING_CONNECTION;
        default:
            return CS_ERROR;
    }
}
