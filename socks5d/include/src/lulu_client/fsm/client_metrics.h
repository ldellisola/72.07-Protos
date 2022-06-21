//
// Created by Lucas Dell'Isola on 20/06/2022.
//

#ifndef SOCKS5D_CLIENT_METRICS_H
#define SOCKS5D_CLIENT_METRICS_H

void LuluClientMetricsWriteRemoteInit(unsigned state, void * data);
unsigned LuluClientMetricsWriteRemoteRun(void * data);

void LuluClientMetricsReadRemoteInit(unsigned state, void * data);
unsigned LuluClientMetricsReadRemoteRun(void * data);
void LuluClientMetricsReadRemoteStop(unsigned state, void * data);


#endif //SOCKS5D_CLIENT_METRICS_H
