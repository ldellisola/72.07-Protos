//
// Created by Lucas Dell'Isola on 17/06/2022.
//

#include "socks5/socks5_users.h"

#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "utils/utils.h"
#include "utils/logger.h"



static Socks5User * currentUsers = null;

static void LoadSingleUser(const char *username, const char *password);

void LoadSocks5Users(const char *usernames[], const char *passwords[]) {
    if (null == usernames)
    {
        Error("users cannot be null");
        return;
    }


    if (null == currentUsers)
        currentUsers = calloc(1, sizeof(Socks5User));

    for (int i = 0; null != usernames[i]; ++i) {
        LoadSingleUser(usernames[i], passwords[i]);
    }
}

Socks5User * LogInSocks5User(const char * username, const char * password) {

    if(null == username || null ==password)
    {
        Error("username and password cannot be null");
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

int DeleteSocks5User(const char *username) {

    if(null == username)
    {
        Error("Username cannot be null");
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
            return OK;
        }
        if(isUser) return LOGGED_IN;
    }

    return DOESNT_EXIST;
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
        Error("User cannot be null");
        return;
    }

    Socks5User * current = null;

    for (current = currentUsers; null != current ; current = current->Next){
        if ((Socks5User *)user == current)
            current->IsLoggedIn = false;
    }
}

void DisposeAllSocks5Users() {
    Debug("Disposing Socks5 users");
    if (null == currentUsers) {
        Warning("Users where not initialized. Cannot dispose them");
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

void LoadSingleUser(const char *username, const char *password) {
    Socks5User * current = null;
    for (current = currentUsers; null != current ; current = current->Next){
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

    current->Next = calloc(1, sizeof(Socks5User));
    current = (Socks5User *) current->Next;

    current->InUse = true;
    size_t usernameLength = strlen(username);
    current->Username = calloc(usernameLength, sizeof(char));
    strncpy(current->Username, username, usernameLength);

    size_t passwordLength = strlen(password);
    current->Password = calloc(passwordLength, sizeof(char));
    strncpy(current->Password, password, passwordLength);
}

