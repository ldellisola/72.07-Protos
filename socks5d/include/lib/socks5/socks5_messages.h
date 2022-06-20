//
// Created by Lucas Dell'Isola on 08/06/2022.
//

#ifndef SOCKS5D_SOCKS5_MESSAGES_H
#define SOCKS5D_SOCKS5_MESSAGES_H

#include <stdbool.h>
#include <netinet/in.h>
#include "utils/utils.h"
#include "parsers/socks5/request_parser.h"

#define SOCKS5_HELLO_RESPONSE_LENGTH (2)
#define SOCKS5_AUTH_RESPONSE_LENGTH (2)
#define SOCKS5_REQUEST_RESPONSE_LENGTH (10)

/**
 * It writes the HelloResponse message to a buffer
 * @param buffer buffer to WriteHead to
 * @param length length of the buffer
 * @param authenticationMethod chosen authentication method
 * @return the length of the message, -1 if the buffer is to small
 */
size_t BuildHelloResponse(byte *buffer, size_t length, int authenticationMethod);

/**
 * It writes the AuthResponse message to a buffer
 * @param buffer buffer to WriteHead to
 * @param length length of the buffer
 * @param authenticationSucceeded Whether the user is authenticated or not
 * @return the length of the message, -1 if the buffer is to small
 */
size_t BuildAuthResponse(byte *buffer, size_t length, bool authenticationSucceeded);

/**
 * It writes the RequestResponse
 * @param buffer buffer to WriteHead to
 * @param length length of the buffer
 * @param replyCommand reply command
 * @param parser RequestParser user to recieve the Data
 * @return the length of the message, -1 if the buffer is to small
 */
size_t BuildRequestResponse(byte *buffer, size_t length, int replyCommand);


#endif //SOCKS5D_SOCKS5_MESSAGES_H
