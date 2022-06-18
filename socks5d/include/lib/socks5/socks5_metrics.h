//
// Created by Lucas Dell'Isola on 18/06/2022.
//

#ifndef SOCKS5D_SOCKS5_METRICS_H
#define SOCKS5D_SOCKS5_METRICS_H

#include <stddef.h>

typedef struct{
    unsigned long HistoricalConnections;
    unsigned long CurrentConnections;
    unsigned long BytesTransferred;
}Socks5Metrics;

/**
 * It initializes all metrics in 0
 */
void InitSocks5Metrics();

/**
 * It registers a new Socks5 connection
 */
void RegisterConnectionInSocks5Metrics();

/**
 * It registers a disconnection from an existing Socks5 connection
 */
void RegisterDisconnectionInSocks5Metrics();

/**
 * It registers the number of bytes registered during a transfer
 * @param bytes Number of bytes transferred
 */
void RegisterBytesTransferredInSocks5Metrics(size_t bytes);

/**
 * It obtains a reference to the current metrics
 * @return pointer to our metrics
 */
Socks5Metrics * GetSocks5Metrics();

#endif //SOCKS5D_SOCKS5_METRICS_H
