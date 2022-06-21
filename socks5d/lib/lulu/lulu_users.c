//
// Created by tluci on 19/6/2022.
//

#include "lulu/lulu_users.h"
#include "utils/logger.h"
#include <assert.h>

LuluUser * currentLuluUsers = null;
void LoadSingleLuluUser(const char *username, const char *password);

LuluUser * LogInLuluUser(const char * username, const char * password) {

    if(null == username || null ==password)
    {
        Warning("username and password cannot be null");
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

void LoadLuluUsers(const char *usernames[], const char *passwords[]) {
    if (null == usernames)
    {
        Warning("users cannot be null");
        return;
    }


    if (null == currentLuluUsers)
        currentLuluUsers = calloc(1, sizeof(LuluUser));

    for (int i = 0; null != usernames[i]; ++i) {
        LoadSingleLuluUser(usernames[i], passwords[i]);
    }
}

void LoadSingleLuluUser(const char *username, const char *password) {
    LuluUser * current = null;
    for (current = currentLuluUsers; null != current ; current = current->Next){
        if (!current->InUse)
        {
            current->InUse = true;
            size_t usernameLength = strlen(username);
            current->Username = calloc(usernameLength+1, sizeof(char));
            strncpy(current->Username, username, usernameLength);

            size_t passwordLength = strlen(password);
            current->Password = calloc(passwordLength+1, sizeof(char));
            strncpy(current->Password, password, passwordLength);

            return;
        }

        if (null == current->Next)
            break;
    }
    assert(current != null);

    current->Next = calloc(1, sizeof(LuluUser));
    current = (LuluUser *) current->Next;

    current->InUse = true;
    size_t usernameLength = strlen(username);
    current->Username = calloc(usernameLength, sizeof(char));
    strncpy(current->Username, username, usernameLength);

    size_t passwordLength = strlen(password);
    current->Password = calloc(passwordLength, sizeof(char));
    strncpy(current->Password, password, passwordLength);
}

void LogOutLuluUser(struct LuluUser *user) {

    if(null == user) {
        Warning("User cannot be null");
        return;
    }

    LuluUser * current = null;

    for (current = currentLuluUsers; null != current ; current = current->Next){
        if ((LuluUser *)user == current)
            current->IsLoggedIn = false;
    }
}

void DisposeAllLuluUsers() {
    Debug("Disposing Socks5 users");
    if (null == currentLuluUsers) {
        Warning("Users where not initialized. Cannot dispose them");
        return;
    }

    LuluUser * next;
    for (LuluUser * user = currentLuluUsers; user != null ; user = next) {
        next = (LuluUser *) user->Next;

        if (null != user->Username)
            free(user->Username);

        if (null != user->Password)
            free(user->Password);

        free(user);
    }

}
