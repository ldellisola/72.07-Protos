//
// Created by Lucas Dell'Isola on 20/06/2022.
//

#ifndef SOCKS5D_CLIENT_AUTH_H
#define SOCKS5D_CLIENT_AUTH_H


void LuluClientAuthWriteInit (unsigned state, void * data);
unsigned LuluClientAuthWriteRun (void * data);
unsigned LuluClientReadRun (void * data);
void LuluClientAuthWriteStop (unsigned state, void * data);


#endif //SOCKS5D_CLIENT_AUTH_H
