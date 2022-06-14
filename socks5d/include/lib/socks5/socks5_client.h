//
// Created by Lucas Dell'Isola on 14/06/2022.
//

#ifndef SOCKS5D_SOCKS5_CLIENT_H
#define SOCKS5D_SOCKS5_CLIENT_H


void ClientReadInit(unsigned state, void * data);

unsigned ClientReadRun(void * data);

void ClientReadClose(unsigned state, void * data);

void ClientWriteInit(unsigned state, void * data);

unsigned ClientWriteRun(void * data);

void ClientWriteClose(unsigned state, void * data);



#endif //SOCKS5D_SOCKS5_CLIENT_H
