//
// Created by Lucas Dell'Isola on 17/06/2022.
//

#include "socks5/socks5_users.h"

#include <stdbool.h>
#include <string.h>
#include "socks5/socks5.h"
#include "utils/utils.h"
#include "utils/logger.h"



Socks5User * currentUsers = null;

void LoadSingleUser(User* user);

void LoadSocks5Users(User * users, int length) {
    if (null == users)
    {
        LogError(false,"users cannot be null");
        return;
    }

    if (length <0)
    {
        LogError(false,"Invalid number of users: %d",length);
        return;
    }

    if (null == currentUsers)
        currentUsers = calloc(1, sizeof(Socks5User));

    for (int i = 0; i < length; ++i) {
        LoadSingleUser(&users[i]);
    }
}

Socks5User * LogInSocks5User(const char * username, const char * password) {

    if(null == username || null ==password)
    {
        LogError(false,"username and password cannot be null");
        return null;
    }

    Socks5User * current = null;
    for (current = currentUsers; null != current ; current = (Socks5User *) current->Next){
        if (!current->InUse)
            continue;
        bool isAuthorized = 0 == strcmp(current->Username,username);
        isAuthorized &= 0 == strcmp(current->Password,password);
        if (isAuthorized)
            return current;
    }

    return null;
}

bool DeleteSocks5User(const char *username) {

    if(null == username)
    {
        LogError(false,"Username cannot be null");
    }

    Socks5User * current = null;
    for (current = currentUsers; null != current ; current = current->Next){
        bool isUser = 0 == strcmp(current->Username,username);
        if (isUser && !current->IsLoggedIn)
        {
            if (null != current->Username)
                free(current->Username);
            current->Username = null;

            if (null != current->Password)
                free(current->Password);
            current->Password = null;

            current->InUse = false;
            current->IsLoggedIn = false;
            return true;
        }
    }

    return false;
}

int GetAllLoggedInSocks5Users(char **usernames, int length) {
    int i = 0;
    for (Socks5User * current = currentUsers; null != current ; current = current->Next){
        if (current->InUse && current->IsLoggedIn) {
            usernames[i++] = current->Username;
            if (i == length)
                return -1;
        }
    }

    return i;
}

void LogOutSocks5User(struct Socks5User *user) {

    if(null == user) {
        LogError(false,"User cannot be null");
        return;
    }

    Socks5User * current = null;

    for (current = currentUsers; null != current ; current = current->Next){
        if ((Socks5User *)user == current)
            current->IsLoggedIn = false;
    }
}

void DisposeAllSocks5Users() {
    LogInfo("Disposing Socks5 users");
    if (null == currentUsers) {
        LogInfo("Users where not initialized. Cannot dispose them");
        return;
    }

    Socks5User * next;
    for (Socks5User * user = currentUsers; user != null ; user = next) {
        next = (Socks5User *) user->Next;

        if (null != user->Username)
            free(user->Username);

        if (null != user->Password)
            free(user->Password);

        free(user);
    }

}

void LoadSingleUser(User* user){
    Socks5User * current = null;
    for (current = currentUsers; null != current ; current = current->Next){
        if (!current->InUse)
        {
            current->InUse = true;
            int usernameLength = strlen(user->Username);
            current->Username = calloc(usernameLength+1, sizeof(char));
            strncpy(current->Username,user->Username,usernameLength);

            int passwordLength = strlen(user->Password);
            current->Password = calloc(passwordLength+1, sizeof(char));
            strncpy(current->Password,user->Password,passwordLength);

            return;
        }

        if (null == current->Next)
            break;
    }

    current->Next = calloc(1, sizeof(Socks5User));
    current = (Socks5User *) current->Next;

    current->InUse = true;
    int usernameLength = strlen(user->Username);
    current->Username = calloc(usernameLength, sizeof(char));
    strncpy(current->Username,user->Username,usernameLength);

    int passwordLength = strlen(user->Password);
    current->Password = calloc(passwordLength, sizeof(char));
    strncpy(current->Password,user->Password,passwordLength);
}

