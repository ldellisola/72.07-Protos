//
// Created by tluci on 11/6/2022.
//

#ifndef SOCKS5D_LULU_SERVER_H
#define SOCKS5D_LULU_SERVER_H

#include "tcp/tcp.h"
#include "lulu/lulu_connection.h"
#include "lulu/lulu_users.h"



bool RegisterLuluServer(const char *port, const char *address, int poolSize,const char *usernames[], const char *passwords[]);
bool RegisterLuluServerOnIPv4(const char * port, const char *address);
bool RegisterLuluServerOnIPv6(const char * port, const char *address);
void DisposeLuluServer();
#endif //SOCKS5D_LULU_SERVER_H
