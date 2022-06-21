//
// Created by Lucas Dell'Isola on 20/06/2022.
//

#include <memory.h>
#include <stdio.h>
#include "tcp/tcp.h"
#include "lulu_client/fsm/client_auth.h"
#include "lulu_client/client_data.h"
#define ATTACHMENT(key) ( (ClientData *)((SelectorKey*)(key))->Data)

void LuluClientAuthWriteInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    BufferReset(&d->Buffer);

    const char * hello = "HELLO|";
    for(size_t i = 0 ; i < strlen(hello);  i++)
        BufferWriteByte(&d->Buffer, hello[i]);

    for(size_t i = 0; i < strlen(d->Username);  i++)
        BufferWriteByte(&d->Buffer, d->Username[i]);

    BufferWriteByte(&d->Buffer, '|');

    for(size_t i = 0; i < strlen(d->Password);  i++)
        BufferWriteByte(&d->Buffer, d->Password[i]);

    BufferWriteByte(&d->Buffer, '\r');
    BufferWriteByte(&d->Buffer, '\n');

}

unsigned LuluClientAuthWriteRun(void *data) {
    ClientData * d = ATTACHMENT(data);

    if (!BufferCanRead(&d->Buffer)){
        int selectorRes = SelectorSetInterestKey(data,SELECTOR_OP_READ);
        return SELECTOR_STATUS_SUCCESS == selectorRes ? CS_READ_AUTH : CS_ERROR;
    }

    size_t len;
    byte * ptr = BufferReadPtr(&d->Buffer,&len);

    ssize_t written = WriteToTcpConnection(d->Remote,ptr,len);

    if (0 == written)
        return CS_ERROR;

    BufferReadAdv(&d->Buffer,written);

    return CS_WRITE_AUTH;
}

void LuluClientAuthWriteStop(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    BufferReset(&d->Buffer);
}

unsigned LuluClientReadRun(void *data) {
    ClientData * d = ATTACHMENT(data);
    SelectorKey* key = (SelectorKey*)(data);

    size_t len;
    byte * ptr = BufferWritePtr(&d->Buffer,&len);
    ssize_t read = ReadFromTcpConnection(d->Remote,ptr,len);

    if (read <= 0)
        return CS_ERROR;

    if ('-' == ptr[0])
    {
        printf("Credentials not valid!");
        return CS_DONE;
    }


    for(ssize_t i = 0 ; i < read-1 ; i++)
        if ('\r' == ptr[i] && '\n' == ptr[i+1]) {
            unsigned selectorRet = SelectorSetInterest(key->Selector,0,SELECTOR_OP_READ);
            selectorRet |= SelectorSetInterestKey(key,SELECTOR_OP_NOOP);
            return  SELECTOR_STATUS_SUCCESS == selectorRet ? CS_TRANSACTION : CS_ERROR;
        }

    return CS_READ_AUTH;
}









