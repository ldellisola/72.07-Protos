//
// Created by Lucas Dell'Isola on 20/06/2022.
//

#include <stdio.h>
#include <memory.h>
#include "lulu_client/fsm/client_timeout.h"
#include "lulu_client/client_data.h"
#include "tcp/tcp.h"

#define ATTACHMENT(key) ( (ClientData *)((SelectorKey*)(key))->Data)

void LuluClientGetTimeoutWriteInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    BufferReset(&d->Buffer);

    const char *  get = "GET|TIMEOUT\r\n";
    for(int i = 0 ; get[i] != 0; i++)
        BufferWriteByte(&d->Buffer,get[i]);

}

unsigned LuluClientGetTimeoutWriteRun(void *data) {
    ClientData * d = ATTACHMENT(data);

    if (!BufferCanRead(&d->Buffer))
    {
        int ss = SelectorSetInterestKey(data,SELECTOR_OP_READ);
        return SELECTOR_STATUS_SUCCESS == ss ? CS_GET_TIMEOUT_READ : CS_ERROR;
    }
    size_t len;
    byte * ptr = BufferReadPtr(&d->Buffer,&len);

    ssize_t written = WriteToTcpConnection(d->Remote,ptr,len);

    if (written <= 0)
        return CS_ERROR;

    BufferReadAdv(&d->Buffer,written);

    return CS_GET_TIMEOUT_WRITE;
}

void LuluClientGetTimeoutReadInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    BufferReset(&d->Buffer);
}

unsigned LuluClientGetTimeoutReadRun(void *data) {
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
            ss |= SelectorSetInterest(key->Selector,0,SELECTOR_OP_READ);
            ss |= SelectorSetInterest(key->Selector,d->Remote->FileDescriptor,SELECTOR_OP_NOOP);

            return SELECTOR_STATUS_SUCCESS == ss ? CS_TRANSACTION : CS_ERROR;
        }
    }

    return CS_GET_TIMEOUT_READ;
}

void LuluClientGetTimeoutReadStop(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);

    char val = BufferReadByte(&d->Buffer);

    if ('+' == val) {
        printf("The timeout is ");

        while (0 != (val = BufferReadByte(&d->Buffer)))
            putchar(val);
        printf("\n");
    }
    else
    {
        printf("Cannot retreive the timeout\n");
    }
}

void LuluClientSetTimeoutReadLocalInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    BufferReset(&d->Buffer);
    printf("Write the new value of the timer: ");
}

unsigned LuluClientSetTimeoutReadLocalRun(void *data) {
    ClientData * d = ATTACHMENT(data);
    SelectorKey * key = data;

    size_t len;
    byte * ptr = BufferWritePtr(&d->Buffer,&len);
    ssize_t readLen = read(0,ptr,len);

    if (readLen <= 0)
        return CS_ERROR;

    BufferWriteAdv(&d->Buffer, readLen-1);

    // todo validar un poco al menos
    if ( '\n' != ptr[readLen-1])
        return CS_SET_TIMEOUT_READ_LOCAL;

    unsigned ss = SELECTOR_STATUS_SUCCESS;
    ss |= SelectorSetInterest(key->Selector,0, SELECTOR_OP_NOOP);
    ss |= SelectorSetInterest(key->Selector,d->Remote->FileDescriptor, SELECTOR_OP_WRITE);

    return SELECTOR_STATUS_SUCCESS == ss ? CS_SET_TIMEOUT_WRITE_REMOTE : CS_ERROR;

}

void LuluClientSetTimeoutWriteRemoteInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);

    size_t len;
    byte * ptr = BufferReadPtr(&d->Buffer,&len);
    char * old = calloc(len, sizeof(char));
    memcpy(old,ptr,len);
    BufferReset(&d->Buffer);

    const char * msg = "SET|TIMEOUT|";
    for(int i = 0; msg[i] != 0; i++)
        BufferWriteByte(&d->Buffer,msg[i]);

    for(size_t i = 0; i < len; i++)
        BufferWriteByte(&d->Buffer,old[i]);

    BufferWriteByte(&d->Buffer,'\r');
    BufferWriteByte(&d->Buffer,'\n');

    free(old);
}

unsigned LuluClientSetTimeoutWriteRemoteRun(void *data) {
    ClientData * d = ATTACHMENT(data);
    SelectorKey * key = data;

    if(!BufferCanRead(&d->Buffer)){
        unsigned ss = SELECTOR_STATUS_SUCCESS;
        ss |= SelectorSetInterest(key->Selector,0, SELECTOR_OP_NOOP);
        ss |= SelectorSetInterest(key->Selector,d->Remote->FileDescriptor, SELECTOR_OP_READ);
        return SELECTOR_STATUS_SUCCESS == ss ? CS_SET_TIMEOUT_READ_REMOTE : CS_ERROR;
    }

    size_t len;
    byte * ptr = BufferReadPtr(&d->Buffer,&len);
    ssize_t written = WriteToTcpConnection(d->Remote,ptr,len);

    if (written <= 0)
        return CS_ERROR;

    BufferReadAdv(&d->Buffer,written);

    return CS_SET_TIMEOUT_WRITE_REMOTE;
}

void LuluClientSetTimeoutReadRemoteInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    BufferReset(&d->Buffer);
}

unsigned LuluClientSetTimeoutReadRemoteRun(void *data) {
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
            ss |= SelectorSetInterest(key->Selector,0,SELECTOR_OP_READ);
            ss |= SelectorSetInterest(key->Selector,d->Remote->FileDescriptor,SELECTOR_OP_NOOP);

            return SELECTOR_STATUS_SUCCESS == ss ? CS_TRANSACTION : CS_ERROR;
        }
    }

    return CS_SET_TIMEOUT_READ_REMOTE;
}

void LuluClientSetTimeoutReadRemoteStop(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);

    char val = BufferReadByte(&d->Buffer);

    if ('+' == val) {
        printf("The value was updated successfuly\n");
    }
    else
    {
        printf("Cannot set the timeout\n");
    }
}






