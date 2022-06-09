//
// Created by Lucas Dell'Isola on 08/06/2022.
//

#ifndef SOCKS5D_SOCKS5_MESSAGES_H
#define SOCKS5D_SOCKS5_MESSAGES_H

#include <stdbool.h>
#include <netinet/in.h>
#include "utils/utils.h"
#include "parsers/request_parser.h"

/**
 * It writes the HelloResponse message to a buffer
 * @param buffer buffer to WriteHead to
 * @param length length of the buffer
 * @param authenticationMethod chosen authentication method
 * @return the length of the message, -1 if the buffer is to small
 */
int BuildHelloResponse(byte * buffer, int length, int authenticationMethod);

/**
 * It writes the AuthResponse message to a buffer
 * @param buffer buffer to WriteHead to
 * @param length length of the buffer
 * @param authenticationSucceeded Whether the user is authenticated or not
 * @return the length of the message, -1 if the buffer is to small
 */
int BuildAuthResponse(byte * buffer, int length, bool authenticationSucceeded);

/**
 * It writes the RequestResponse with an IPv4 address
 * @param buffer buffer to WriteHead to
 * @param length length of the buffer
 * @param replyCommand replyCommand command
 * @param address destination IPv4 address composed of 4 bytes
 * @param port byte array of two elements with the port'Selector bytes in network order
 * @return the length of the message, -1 if the buffer is to small
 */
int BuildRequestResponseWithIPv4(byte * buffer, int length, int replyCommand, const byte* address, const byte * port);

/**
 * It writes the RequestResponse with an IPv6 address
 * @param buffer buffer to WriteHead to
 * @param length length of the buffer
 * @param replyCommand replyCommand command
 * @param address destination IPv6 address composed of 16 bytes
 * @param port byte array of two elements with the port'Selector bytes in network order
 * @return the length of the message, -1 if the buffer is to small
 */
int BuildRequestResponseWithIPv6(byte * buffer, int length, int replyCommand, const byte* address, const byte *port);

/**
 * It writes the RequestResponse with an FQDN address
 * @param buffer buffer to WriteHead to
 * @param length length of the buffer
 * @param replyCommand reply command
 * @param address destination FQDN as a null-terminated string
 * @param port byte array of two elements with the port'Selector bytes in network order
 * @return the length of the message, -1 if the buffer is to small
 */
int BuildRequestResponseWithFQDN(byte * buffer, int length, int replyCommand, const char * address, const byte * port);

/**
 * It writes the RequestResponse
 * @param buffer buffer to WriteHead to
 * @param length length of the buffer
 * @param replyCommand reply command
 * @param addressType IPv4, IPv6 or FQDN
 * @param address destination address. It can be a FQDN as a null-terminated string, a IPv4 address as a 4 byte array or a IPv6 address as a 16 byte array
 * @param port byte array of two elements with the port'Selector bytes in network order
 * @return the length of the message, -1 if the buffer is to small
 */
int BuildRequestResponse(byte * buffer, int length, int replyCommand,int addressType , void * address, const byte * port);

/**
 * It writes the RequestResponse
 * @param buffer buffer to WriteHead to
 * @param length length of the buffer
 * @param replyCommand reply command
 * @param parser RequestParser user to recieve the Data
 * @return the length of the message, -1 if the buffer is to small
 */
int BuildRequestResponseFromParser(byte * buffer, int length, int replyCommand, RequestParser * parser);


#endif //SOCKS5D_SOCKS5_MESSAGES_H
