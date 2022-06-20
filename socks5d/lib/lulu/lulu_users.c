//
// Created by tluci on 19/6/2022.
//

#include "lulu/lulu_users.h"
#include "utils/logger.h"

LuluUser * currentLuluUsers = null;

LuluUser * LogInLuluUser(const char * username, const char * password) {

    if(null == username || null ==password)
    {
        Error("username and password cannot be null");
        return null;
    }

    LuluUser * current = null;
    for (current = currentLuluUsers; null != current ; current = (LuluUser *) current->Next){
        if (!current->InUse)
            continue;
        bool isAuthorized = 0 == strcmp(current->Username,username);
        isAuthorized &= 0 == strcmp(current->Password,password);
        if (isAuthorized)
            return current;
    }

    return null;
}
