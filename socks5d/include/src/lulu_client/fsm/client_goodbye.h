//
// Created by Lucas Dell'Isola on 21/06/2022.
//

#ifndef SOCKS5D_CLIENT_GOODBYE_H
#define SOCKS5D_CLIENT_GOODBYE_H

void LuluClientGoodbyeWriteRemoteInit(unsigned state, void * data);
unsigned LuluClientGoodbyeWriteRemoteRun(void *data);


void LuluClientGoodbyeReadInit(unsigned state, void * data);
unsigned LuluClientGoodbyeReadRun(void *data);
void LuluClientGoodbyeReadStop(unsigned state, void * data);

#endif //SOCKS5D_CLIENT_GOODBYE_H
