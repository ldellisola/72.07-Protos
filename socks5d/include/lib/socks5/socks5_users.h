//
// Created by Lucas Dell'Isola on 17/06/2022.
//

#ifndef SOCKS5D_SOCKS5_USERS_H
#define SOCKS5D_SOCKS5_USERS_H
#define LOGGED_IN   2
#define OK  0
#define DOESNT_EXIST 1
#include <stdbool.h>
#include "socks5.h"

typedef struct Socks5User{
    struct Socks5User * Next;
    char * Username;
    char * Password;
    bool InUse;
    bool IsLoggedIn;
}Socks5User;


/**
 * It loads to memory a set of users
 * @param usernames usernames to add. The last element must be null
 * @param passwords passwords to add. The last element must be null
 */
void LoadSocks5Users(const char *usernames[], const char *passwords[]);

/**
 * It checks if a user is authorized to use the server
 * @param username username of the user
 * @param password password of the user
 * @return the user if it is authorized, else null
 */
Socks5User * LogInSocks5User(const char * username, const char * password);

/**
 * It logs out a user from the socks5 server
 * @param user user to log out.
 */
void LogOutSocks5User(struct Socks5User * user);

/**
 * It deletes a user. If the user is logged in, then it cannot be erased.
 * @param username username to delete
 * @return 0 deleted, 1 error doesnt exist, 2 error logged in
 */
int DeleteSocks5User(const char * username);

/**
 * It retrieves all the currently logged users.
 * @param usernames Pointer to a list of strings to store the usernames
 * @param length max size of usernames we can store in the previous array
 * @return The number of usernames retrieved. -1 means that there MAY be more logged users
 */
int GetAllLoggedInSocks5Users(char ** usernames, int length);


/**
 * It safely disposes the memory allocated by the users module
 */
void DisposeAllSocks5Users();


#endif //SOCKS5D_SOCKS5_USERS_H
