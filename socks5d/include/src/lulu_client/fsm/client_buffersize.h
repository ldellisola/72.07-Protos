//
// Created by Lucas Dell'Isola on 20/06/2022.
//

#ifndef SOCKS5D_CLIENT_BUFFERSIZE_H
#define SOCKS5D_CLIENT_BUFFERSIZE_H

void LuluClientGetBufferSizeWriteInit(unsigned state, void * data);

unsigned LuluClientGetBufferSizeWriteRun(void * data);

void LuluClientGetBufferSizeReadInit(unsigned state, void * data);

unsigned LuluClientGetBufferSizeReadRun(void * data);

void LuluClientGetBufferSizeReadStop(unsigned state, void * data);

void LuluClientSetBufferSizeReadLocalInit (unsigned state, void * data);

unsigned LuluClientSetBufferSizeReadLocalRun(void * data);

void LuluClientSetBufferSizeWriteRemoteInit(unsigned state, void * data);

unsigned LuluClientSetBufferSizeWriteRemoteRun(void * data);

void LuluClientSetBufferSizeReadRemoteInit(unsigned state, void * data);

unsigned LuluClientSetBufferSizeReadRemoteRun(void * data);

void LuluClientSetBufferSizeReadRemoteStop(unsigned state, void * data);

#endif //SOCKS5D_CLIENT_BUFFERSIZE_H
