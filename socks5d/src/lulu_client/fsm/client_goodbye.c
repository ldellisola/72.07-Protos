//
// Created by Lucas Dell'Isola on 21/06/2022.
//
#include <stdio.h>
#include "lulu_client/fsm/client_goodbye.h"
#include "lulu_client/client_data.h"
#include "tcp/tcp.h"

#define ATTACHMENT(key) ( (ClientData *)((SelectorKey*)(key))->Data)

void LuluClientGoodbyeWriteRemoteInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    BufferReset(&d->Buffer);

    const char *  get = "GOODBYE\r\n";
    for(int i = 0 ; get[i] != 0; i++)
        BufferWriteByte(&d->Buffer,get[i]);
}

unsigned LuluClientGoodbyeWriteRemoteRun(void *data) {
    ClientData * d = ATTACHMENT(data);

    if (!BufferCanRead(&d->Buffer))
    {
        int ss = SelectorSetInterestKey(data,SELECTOR_OP_READ);

        return SELECTOR_STATUS_SUCCESS == ss ? CS_GOODBYE_READ_REMOTE : CS_ERROR;
    }
    size_t len;
    byte * ptr = BufferReadPtr(&d->Buffer,&len);

    ssize_t written = WriteToTcpConnection(d->Remote,ptr,len);

    if (written <= 0)
        return CS_ERROR;

    BufferReadAdv(&d->Buffer,written);

    return CS_GOODBYE_WRITE_REMOTE;
}

void LuluClientGoodbyeReadInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    BufferReset(&d->Buffer);
}

unsigned LuluClientGoodbyeReadRun(void *data) {
    ClientData * d = ATTACHMENT(data);
    SelectorKey * key = data;

    size_t len;
    byte * ptr = BufferWritePtr(&d->Buffer,&len);

    ssize_t read = ReadFromTcpConnection(d->Remote,ptr,len);

    if (read <= 0)
        return CS_ERROR;

    BufferWriteAdv(&d->Buffer,read);

    for (int i = 0; i < read-1 ; ++i) {
        if ('\r' == ptr[i] && '\n' == ptr[i+1]) {
            unsigned ss = SELECTOR_STATUS_SUCCESS;
            ss |= SelectorSetInterest(key->Selector,0,SELECTOR_OP_NOOP);
            ss |= SelectorSetInterest(key->Selector,d->Remote->FileDescriptor,SELECTOR_OP_NOOP);

            return SELECTOR_STATUS_SUCCESS == ss ? CS_DONE : CS_ERROR;
        }
    }

    return CS_GOODBYE_READ_REMOTE;
}

void LuluClientGoodbyeReadStop(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    SelectorKey * key = data;

    char val = BufferReadByte(&d->Buffer);
    if ('+' == val)
        printf("Disconnected successfuly\n");
    else
        printf("There was an error disconnecting from the server\n");

    DisposeTcpConnection(d->Remote,key->Selector);

}
