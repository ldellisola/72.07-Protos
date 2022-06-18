//
// Created by Lucas Dell'Isola on 18/06/2022.
//

#ifndef SOCKS5D_SOCKS5_PASSWORD_DISSECTOR_H
#define SOCKS5D_SOCKS5_PASSWORD_DISSECTOR_H
#include <stdbool.h>
#include "utils/utils.h"
#include "parsers/pop3/pop3_auth_parser.h"
#include "socks5_connection.h"

/**
 * It enables or disables the password dissector on POP3 connections
 * @param enable whether to enable or not
 */
void EnablePasswordDissector(bool enable);

/**
 * It checks if it can detect passwords in the connection's protocol
 * @param connection connection
 * @return True or false
 */
bool CanDetectPasswords(Socks5Connection * connection);

/**
 * It checks if the password dissectors are enabled
 * @return Whether the password dissectors are enables or not
 */
bool ArePasswordDissectorsEnabled();

/**
 * It scans a given array of data for POP3 passwords and prints them if
 * @param data Place to look for passwords
 * @param dataLength Length of data
 * @param parser Parser
 * @return A pointer to the start of the password. If the password could not be found, it returns null
 */
bool ScanForPOP3Passwords(byte *data, size_t dataLength, Pop3AuthParser *parser, char **username, char **password);

#endif //SOCKS5D_SOCKS5_PASSWORD_DISSECTOR_H
