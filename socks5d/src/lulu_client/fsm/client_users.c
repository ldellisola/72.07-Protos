//
// Created by Lucas Dell'Isola on 21/06/2022.
//
#include <stdio.h>
#include <memory.h>
#include "lulu_client/fsm/client_users.h"
#include "lulu_client/client_data.h"
#include "tcp/tcp.h"

#define ATTACHMENT(key) ( (ClientData *)((SelectorKey*)(key))->Data)

void LuluClientSetUserReadLocalInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    BufferReset(&d->Buffer);
    d->Data = 0;
    printf("Write new user and password separated by '|' (for example \"<username>|<pass>\") ");
}

unsigned LuluClientSetUserReadLocalRun(void *data) {
    ClientData * d = ATTACHMENT(data);
    SelectorKey * key = data;

    size_t len;
    byte * ptr = BufferWritePtr(&d->Buffer,&len);
    ssize_t readLen = read(0,ptr,len);

    if (readLen <= 0)
        return CS_ERROR;

    BufferWriteAdv(&d->Buffer, readLen-1);

    long pipes= (long ) d->Data;

    for(ssize_t i = 0 ; i < readLen; i++)
        if ('|' == ptr[i])
            pipes++;

    d->Data = (void *) pipes;

    if ( '\n' != ptr[readLen-1])
        return CS_SET_USER_READ_LOCAL;

    unsigned ss = SELECTOR_STATUS_SUCCESS;


    if (d->Data != (void *)1)
    {
        ss |= SelectorSetInterest(key->Selector,0, SELECTOR_OP_READ);
        ss |= SelectorSetInterest(key->Selector,d->Remote->FileDescriptor, SELECTOR_OP_NOOP);

        printf("Invalid username or password!\n");
        return SELECTOR_STATUS_SUCCESS == ss ? CS_TRANSACTION : CS_ERROR;
    }

    ss |= SelectorSetInterest(key->Selector,0, SELECTOR_OP_NOOP);
    ss |= SelectorSetInterest(key->Selector,d->Remote->FileDescriptor, SELECTOR_OP_WRITE);
    return SELECTOR_STATUS_SUCCESS == ss ? CS_SET_USER_WRITE_REMOTE : CS_ERROR;
}

void LuluClientSetUserWriteRemoteInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);

    size_t len;
    byte * ptr = BufferReadPtr(&d->Buffer,&len);
    char * old = calloc(len, sizeof(char));
    memcpy(old,ptr,len);
    BufferReset(&d->Buffer);

    const char * msg = "SET|USER|";
    for(int i = 0; msg[i] != 0; i++)
        BufferWriteByte(&d->Buffer,msg[i]);

    for(size_t i = 0; i < len; i++)
        BufferWriteByte(&d->Buffer,old[i]);

    BufferWriteByte(&d->Buffer,'\r');
    BufferWriteByte(&d->Buffer,'\n');

    free(old);
}

unsigned LuluClientSetUserWriteRemoteRun(void *data) {
    ClientData * d = ATTACHMENT(data);
    SelectorKey * key = data;

    if(!BufferCanRead(&d->Buffer)){
        unsigned ss = SELECTOR_STATUS_SUCCESS;
        ss |= SelectorSetInterest(key->Selector,0, SELECTOR_OP_NOOP);
        ss |= SelectorSetInterest(key->Selector,d->Remote->FileDescriptor, SELECTOR_OP_READ);
        return SELECTOR_STATUS_SUCCESS == ss ? CS_SET_USER_READ_REMOTE : CS_ERROR;
    }

    size_t len;
    byte * ptr = BufferReadPtr(&d->Buffer,&len);
    ssize_t written = WriteToTcpConnection(d->Remote,ptr,len);

    if (written <= 0)
        return CS_ERROR;

    BufferReadAdv(&d->Buffer,written);

    return CS_SET_USER_WRITE_REMOTE;
}

void LuluClientSetUserReadRemoteInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    BufferReset(&d->Buffer);
}

unsigned LuluClientSetUserReadRemoteRun(void *data) {
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

    return CS_SET_USER_READ_REMOTE;
}

void LuluClientSetUserReadRemoteStop(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    char val = BufferReadByte(&d->Buffer);
    if ('+' == val)
        printf("User added successfuly\n");
    else
        printf("There was an error adding the user\n");

}


///////////////////////////////////////////////////////////////

void LuluClientDelUserReadLocalInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    BufferReset(&d->Buffer);
    printf("Write the username to delete ");
}

unsigned LuluClientDelUserReadLocalRun(void *data) {
    ClientData * d = ATTACHMENT(data);
    SelectorKey * key = data;

    size_t len;
    byte * ptr = BufferWritePtr(&d->Buffer,&len);
    ssize_t readLen = read(0,ptr,len);

    if (readLen <= 0)
        return CS_ERROR;

    BufferWriteAdv(&d->Buffer, readLen-1);
    // todo algo de validacion?
    if ( '\n' != ptr[readLen-1])
        return CS_DEL_USER_READ_LOCAL;

    unsigned ss = SELECTOR_STATUS_SUCCESS;
    ss |= SelectorSetInterest(key->Selector,0, SELECTOR_OP_NOOP);
    ss |= SelectorSetInterest(key->Selector,d->Remote->FileDescriptor, SELECTOR_OP_WRITE);

    return SELECTOR_STATUS_SUCCESS == ss ? CS_DEL_USER_WRITE_REMOTE : CS_ERROR;
}

void LuluClientDelUserWriteRemoteInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);

    size_t len;
    byte * ptr = BufferReadPtr(&d->Buffer,&len);
    char * old = calloc(len, sizeof(char));
    memcpy(old,ptr,len);
    BufferReset(&d->Buffer);

    const char * msg = "DEL|USER|";
    for(int i = 0; msg[i] != 0; i++)
        BufferWriteByte(&d->Buffer,msg[i]);

    for(size_t i = 0; i < len; i++)
        BufferWriteByte(&d->Buffer,old[i]);

    BufferWriteByte(&d->Buffer,'\r');
    BufferWriteByte(&d->Buffer,'\n');

    free(old);
}

unsigned LuluClientDelUserWriteRemoteRun(void *data) {
    ClientData * d = ATTACHMENT(data);
    SelectorKey * key = data;

    if(!BufferCanRead(&d->Buffer)){
        unsigned ss = SELECTOR_STATUS_SUCCESS;
        ss |= SelectorSetInterest(key->Selector,0, SELECTOR_OP_NOOP);
        ss |= SelectorSetInterest(key->Selector,d->Remote->FileDescriptor, SELECTOR_OP_READ);
        return SELECTOR_STATUS_SUCCESS == ss ? CS_DEL_USER_READ_REMOTE : CS_ERROR;
    }

    size_t len;
    byte * ptr = BufferReadPtr(&d->Buffer,&len);
    ssize_t written = WriteToTcpConnection(d->Remote,ptr,len);

    if (written <= 0)
        return CS_ERROR;

    BufferReadAdv(&d->Buffer,written);

    return CS_DEL_USER_WRITE_REMOTE;
}

void LuluClientDelUserReadRemoteInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    BufferReset(&d->Buffer);
}

unsigned LuluClientDelUserReadRemoteRun(void *data) {
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

    return CS_DEL_USER_READ_REMOTE;
}

void LuluClientDelUserReadRemoteStop(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    char val = BufferReadByte(&d->Buffer);
    if ('+' == val)
        printf("User deleted successfuly\n");
    else
        printf("There was an error deleting the user\n");
}

////////////////////////////////////////////////

void LuluClientListUserWriteRemoteInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    BufferReset(&d->Buffer);

    const char *  get = "LIST|USERS\r\n";
    for(int i = 0 ; get[i] != 0; i++)
        BufferWriteByte(&d->Buffer,get[i]);
}

unsigned LuluClientListUserWriteRemoteRun(void *data) {
    ClientData * d = ATTACHMENT(data);

    if (!BufferCanRead(&d->Buffer))
    {
        int ss = SelectorSetInterestKey(data,SELECTOR_OP_READ);
        return SELECTOR_STATUS_SUCCESS == ss ? CS_LIST_USER_READ_REMOTE : CS_ERROR;
    }
    size_t len;
    byte * ptr = BufferReadPtr(&d->Buffer,&len);

    ssize_t written = WriteToTcpConnection(d->Remote,ptr,len);

    if (written <= 0)
        return CS_ERROR;

    BufferReadAdv(&d->Buffer,written);

    return CS_LIST_USER_WRITE_REMOTE;
}

void LuluClientListUserReadInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    BufferReset(&d->Buffer);

}

unsigned LuluClientListUserReadRun(void *data) {
    ClientData * d = ATTACHMENT(data);
    SelectorKey * key = data;

    size_t len;
    byte * ptr = BufferWritePtr(&d->Buffer,&len);

    ssize_t read = ReadFromTcpConnection(d->Remote,ptr,len);

    if (read <= 0)
        return CS_ERROR;

    BufferWriteAdv(&d->Buffer,read);

    if ('-' == ptr[0]){
        printf("There was an error getting the list of connected users\n");
        unsigned ss = SELECTOR_STATUS_SUCCESS;
        ss |= SelectorSetInterest(key->Selector,0,SELECTOR_OP_READ);
        ss |= SelectorSetInterest(key->Selector,d->Remote->FileDescriptor,SELECTOR_OP_NOOP);

        return SELECTOR_STATUS_SUCCESS == ss ? CS_TRANSACTION : CS_ERROR;
    }

    for (int i = 1; i < read-2 ; ++i) {
        if('|' == ptr[i])
            putchar('\n');
        else
            putchar(ptr[i]);


        if ('\r' == ptr[i+1] && '\n' == ptr[i+2]) {
            putchar('\n');
            unsigned ss = SELECTOR_STATUS_SUCCESS;
            ss |= SelectorSetInterest(key->Selector,0,SELECTOR_OP_READ);
            ss |= SelectorSetInterest(key->Selector,d->Remote->FileDescriptor,SELECTOR_OP_NOOP);

            return SELECTOR_STATUS_SUCCESS == ss ? CS_TRANSACTION : CS_ERROR;
        }
    }

    return CS_LIST_USER_READ_REMOTE;
}
