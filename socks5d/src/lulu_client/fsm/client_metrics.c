//
// Created by Lucas Dell'Isola on 20/06/2022.
//

#include <stdio.h>
#include "lulu_client/fsm/client_metrics.h"
#include "lulu_client/client_data.h"
#include "tcp/tcp.h"

#define ATTACHMENT(key) ( (ClientData *)((SelectorKey*)(key))->Data)


void LuluClientMetricsWriteRemoteInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    BufferReset(&d->Buffer);
    const char *  msg = "GET|METRICS\r\n";

    for (int i = 0; 0 != msg[i] ; ++i)
        BufferWriteByte(&d->Buffer,msg[i]);

}

unsigned LuluClientMetricsWriteRemoteRun(void *data) {
    ClientData * d = ATTACHMENT(data);

    if (!BufferCanRead(&d->Buffer))
    {
        int ss = SelectorSetInterestKey(data,SELECTOR_OP_READ);
        return SELECTOR_STATUS_SUCCESS == ss ? CS_METRICS_READ_REMOTE : CS_ERROR;
    }
    size_t len;
    byte * ptr = BufferReadPtr(&d->Buffer,&len);
    ssize_t written = WriteToTcpConnection(d->Remote,ptr,len);

    if (written <= 0)
        return CS_ERROR;

    BufferReadAdv(&d->Buffer,written);

    return CS_METRICS_WRITE_REMOTE;
}

typedef struct {
    long CurrentConnections;
    long AllConnections;
    long BytesTransferred;
    int number;
}MetricsData;

void LuluClientMetricsReadRemoteInit(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    BufferReset(&d->Buffer);
    d->Data = calloc(1, sizeof(MetricsData));

}



unsigned LuluClientMetricsReadRemoteRun(void *data) {
    ClientData * d = ATTACHMENT(data);
    SelectorKey * key = data;

    size_t len;
    byte * ptr = BufferWritePtr(&d->Buffer,&len);

    ssize_t read = ReadFromTcpConnection(d->Remote,ptr,len);

    if (read <= 0)
        return CS_ERROR;

    BufferWriteAdv(&d->Buffer,read);

    if ('-' == ptr[0]){
        unsigned ss = SELECTOR_STATUS_SUCCESS;
        ss |= SelectorSetInterest(key->Selector,0,SELECTOR_OP_READ);
        ss |= SelectorSetInterest(key->Selector,d->Remote->FileDescriptor,SELECTOR_OP_NOOP);

        return SELECTOR_STATUS_SUCCESS == ss ? CS_TRANSACTION : CS_ERROR;
    }

    MetricsData * m = d->Data;

    for (int i = 1; i < read-2 ; ++i) {

        if ('|' == ptr[i]) {
            m->number++;
            continue;
        }

        switch (m->number) {
            case 0:
                m->AllConnections = m->AllConnections * 10  + (ptr[i] - '0');
                break;
            case 1:
                m->CurrentConnections = m->CurrentConnections * 10 + (ptr[i] - '0');
                break;
            case 2:
                m->BytesTransferred = m->BytesTransferred * 10 + (ptr[i]-'0');
        }


        if ('\r' == ptr[i+1] && '\n' == ptr[i+2]) {
            unsigned ss = SELECTOR_STATUS_SUCCESS;
            ss |= SelectorSetInterest(key->Selector,0,SELECTOR_OP_READ);
            ss |= SelectorSetInterest(key->Selector,d->Remote->FileDescriptor,SELECTOR_OP_NOOP);

            return SELECTOR_STATUS_SUCCESS == ss ? CS_TRANSACTION : CS_ERROR;
        }
    }

    return CS_METRICS_READ_REMOTE;
}

void LuluClientMetricsReadRemoteStop(unsigned int state, void *data) {
    ClientData * d = ATTACHMENT(data);
    MetricsData * m = d->Data;

    if (m->number != 2){
        printf("Error while getting metrics\n");
    } else
    {
        printf("Bytes Transferidos: %ld\n",m->BytesTransferred);
        printf("Conexiones Activas: %ld\n",m->CurrentConnections);
        printf("Conexiones Totales: %ld\n",m->AllConnections);

    }
    free(d->Data);


}
