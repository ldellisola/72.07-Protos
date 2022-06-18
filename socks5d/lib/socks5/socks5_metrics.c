//
// Created by Lucas Dell'Isola on 18/06/2022.
//

#include <memory.h>
#include "socks5/socks5_metrics.h"

Socks5Metrics metrics;

void InitSocks5Metrics() {
    memset(&metrics,0, sizeof(Socks5Metrics));
}

void RegisterConnectionInSocks5Metrics() {
    metrics.HistoricalConnections++;
    metrics.CurrentConnections++;
}

void RegisterDisconnectionInSocks5Metrics() {
    metrics.CurrentConnections--;
}

void RegisterBytesTransferredInSocks5Metrics(size_t bytes) {
    metrics.BytesTransferred += bytes;
}

Socks5Metrics * GetSocks5Metrics() {
    return &metrics;
}
