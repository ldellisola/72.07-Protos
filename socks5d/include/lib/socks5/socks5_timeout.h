//
// Created by Lucas Dell'Isola on 20/06/2022.
//

#ifndef SOCKS5D_SOCKS5_TIMEOUT_H
#define SOCKS5D_SOCKS5_TIMEOUT_H

#include "selector/selector.h"

/**
 * It will check all current active socks5 connections and see if they are currently timed out
 * @param fdSelector Selector
 */
void CheckForTimeoutInSocks5Connections(fd_selector fdSelector);

/**
 * It updates the LastConnectionOn field
 * @param data pointer to Socks5Connection
 */
void NotifySocks5ConnectionAccess(void * data);

/**
 * It sets the time it takes for a connection to timeout
 * @param timeout Time in seconds. If its 0 or negative, then connections will never timeout
 */
void SetSocks5ConnectionTimeout(time_t timeout);

/**
 * It gets the time it takes for a connection to timeout
 * If its 0 or negative, then connections will never timeout
 */
time_t GetSocks5ConnectionTimeout();

#endif //SOCKS5D_SOCKS5_TIMEOUT_H
