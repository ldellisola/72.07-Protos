//
// Created by Lucas Dell'Isola on 20/06/2022.
//

#ifndef SOCKS5D_CLIENT_TIMEOUT_H
#define SOCKS5D_CLIENT_TIMEOUT_H

void LuluClientGetTimeoutWriteInit(unsigned state, void * data);

unsigned LuluClientGetTimeoutWriteRun(void * data);

void LuluClientGetTimeoutReadInit(unsigned state, void * data);

unsigned LuluClientGetTimeoutReadRun(void * data);

void LuluClientGetTimeoutReadStop(unsigned state, void * data);

void LuluClientSetTimeoutReadLocalInit (unsigned state, void * data);

unsigned LuluClientSetTimeoutReadLocalRun(void * data);

void LuluClientSetTimeoutWriteRemoteInit(unsigned state, void * data);

unsigned LuluClientSetTimeoutWriteRemoteRun(void * data);

void LuluClientSetTimeoutReadRemoteInit(unsigned state, void * data);

unsigned LuluClientSetTimeoutReadRemoteRun(void * data);

void LuluClientSetTimeoutReadRemoteStop(unsigned state, void * data);

#endif //SOCKS5D_CLIENT_TIMEOUT_H
