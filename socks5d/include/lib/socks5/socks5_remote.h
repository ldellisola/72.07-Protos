//
// Created by Lucas Dell'Isola on 14/06/2022.
//

#ifndef SOCKS5D_SOCKS5_REMOTE_H
#define SOCKS5D_SOCKS5_REMOTE_H

void RemoteReadInit(unsigned state, void *data);

unsigned RemoteReadRun(void *data);

void RemoteReadClose(unsigned state, void *data);


void RemoteWriteInit(unsigned state, void *data);

unsigned RemoteWriteRun(void *data);

void RemoteWriteClose(unsigned state, void *data);

#endif //SOCKS5D_SOCKS5_REMOTE_H
