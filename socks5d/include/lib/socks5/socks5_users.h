//
// Created by Lucas Dell'Isola on 17/06/2022.
//

#ifndef SOCKS5D_SOCKS5_USERS_H
#define SOCKS5D_SOCKS5_USERS_H

#include <stdbool.h>
#include "socks5.h"

struct Socks5User;

/**
 * It loads to memory a set of users
 * @param users Users to add
 * @param length Number of users
 */
void LoadSocks5Users(User* users, int length);

/**
 * It checks if a user is authorized to use the server
 * @param username username of the user
 * @param password password of the user
 * @return the user if it is authorized, else null
 */
struct Socks5User * LogInSocks5User(const char * username, const char * password);

/**
 * It logs out a user from the socks5 server
 * @param user user to log out.
 */
void LogOutSocks5User(struct Socks5User * user);

/**
 * It deletes a user. If the user is logged in, then it cannot be erased.
 * @param username username to delete
 * @return Whether the user was deleted or not
 */
bool DeleteSocks5User(const char * username);

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
