//
// Created by Lucas Dell'Isola on 21/06/2022.
//

#ifndef SOCKS5D_CLIENT_USERS_H
#define SOCKS5D_CLIENT_USERS_H

void LuluClientSetUserReadLocalInit(unsigned state, void * data);
unsigned LuluClientSetUserReadLocalRun (void *data);

void LuluClientSetUserWriteRemoteInit(unsigned state, void * data);
unsigned LuluClientSetUserWriteRemoteRun(void * data);

void LuluClientSetUserReadRemoteInit(unsigned state, void * data);
unsigned LuluClientSetUserReadRemoteRun(void * data);
void LuluClientSetUserReadRemoteStop(unsigned state, void * data);

void LuluClientDelUserReadLocalInit(unsigned state, void * data);
unsigned LuluClientDelUserReadLocalRun (void *data);

void LuluClientDelUserWriteRemoteInit(unsigned state, void * data);
unsigned LuluClientDelUserWriteRemoteRun(void * data);

void LuluClientDelUserReadRemoteInit(unsigned state, void * data);
unsigned LuluClientDelUserReadRemoteRun(void * data);
void LuluClientDelUserReadRemoteStop(unsigned state, void * data);

void LuluClientListUserWriteRemoteInit(unsigned state,void * data);
unsigned LuluClientListUserWriteRemoteRun(void * data);

void LuluClientListUserReadInit(unsigned state, void * data);
unsigned LuluClientListUserReadRun(void * data);

#endif //SOCKS5D_CLIENT_USERS_H
