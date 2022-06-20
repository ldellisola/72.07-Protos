//
// Created by tluci on 19/6/2022.
//

#ifndef SOCKS5D_LULU_USERS_H
#define SOCKS5D_LULU_USERS_H
#include "lulu_server.h"

typedef struct LuluUser{
    struct LuluUser * Next;
    char * Username;
    char * Password;
    bool InUse;
    bool IsLoggedIn;
}LuluUser;
LuluUser * LogInLuluUser(const char * username, const char * password);
#endif //SOCKS5D_LULU_USERS_H
