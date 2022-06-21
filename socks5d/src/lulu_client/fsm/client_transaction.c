//
// Created by Lucas Dell'Isola on 20/06/2022.
//

#include <stdio.h>
#include "lulu_client/fsm/client_transaction.h"
#include "lulu_client/client_data.h"
#include "tcp/tcp.h"

#define ATTACHMENT(key) ( (ClientData *)((SelectorKey*)(key))->Data)

void LuluClientTransactionInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    BufferReset(&d->Buffer);

    printf("What do you want to do?\n");
    printf("\t1. Get timeout\n");
    printf("\t2. Set timeout\n");
    printf("\t3. Get buffer size\n");
    printf("\t4. Set buffer size\n");
    printf("\t5. Get metrics\n");
    printf("\t6. Create user\n");
    printf("\t7. Delete user\n");
    printf("\t8. List users\n");
    printf("\tq. Quit\n");
}

unsigned LuluClientTransactionRead(void *data) {
    ClientData * d = ATTACHMENT(data);
    SelectorKey* key = data;

    size_t len;
    byte * ptr = BufferWritePtr(&d->Buffer,&len);
    ssize_t readval = read(0,ptr,len);

    if (readval <= 0)
        return CS_ERROR;

    if (readval > 2) {
        printf("Invalid character");
        return CS_TRANSACTION;
    }

    int stdInMode;
    int remoteMode;
    unsigned state;

    switch (ptr[0]) {
        case '1':
            stdInMode = SELECTOR_OP_NOOP;
            remoteMode = SELECTOR_OP_WRITE;
            state = CS_GET_TIMEOUT_WRITE;
            break;
        case '2':
            stdInMode = SELECTOR_OP_READ;
            remoteMode = SELECTOR_OP_NOOP;
            state = CS_SET_TIMEOUT_READ_LOCAL;
            break;
        case '3':
            stdInMode = SELECTOR_OP_NOOP;
            remoteMode = SELECTOR_OP_WRITE;
            state = CS_GET_BUFFERSIZE_WRITE;
            break;
        case '4':
            stdInMode = SELECTOR_OP_READ;
            remoteMode = SELECTOR_OP_NOOP;
            state = CS_SET_BUFFERSIZE_READ_LOCAL;
            break;
        case '5':
            stdInMode = SELECTOR_OP_NOOP;
            remoteMode = SELECTOR_OP_WRITE;
            state = CS_METRICS_WRITE_REMOTE;
            break;
        case '6':
            stdInMode = SELECTOR_OP_READ;
            remoteMode = SELECTOR_OP_NOOP;
            state = CS_SET_USER_READ_LOCAL;
            break;
        case '7':
            stdInMode = SELECTOR_OP_READ;
            remoteMode = SELECTOR_OP_NOOP;
            state = CS_DEL_USER_READ_LOCAL;
            break;
        case '8':
            stdInMode = SELECTOR_OP_NOOP;
            remoteMode = SELECTOR_OP_WRITE;
            state = CS_LIST_USER_WRITE_REMOTE;
            break;
        case 'q':
            stdInMode = SELECTOR_OP_NOOP;
            remoteMode = SELECTOR_OP_WRITE;
            state = CS_GOODBYE_WRITE_REMOTE;
            break;
        default:
            printf("Invalid character\n");
            return CS_TRANSACTION;
    }

    unsigned selectorStatus = SELECTOR_STATUS_SUCCESS;
    selectorStatus |= SelectorSetInterest(key->Selector,0, stdInMode);
    selectorStatus |= SelectorSetInterest(key->Selector,d->Remote->FileDescriptor, remoteMode);

    return SELECTOR_STATUS_SUCCESS == selectorStatus ? state : CS_ERROR;
}














