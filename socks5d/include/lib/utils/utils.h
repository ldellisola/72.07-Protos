//
// Created by Lucas Dell'Isola on 28/05/2022.
//
#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <stdint.h>
#include <stdlib.h>
#include <netdb.h>

#define null NULL
#define OK 0
#define ERROR -1


typedef uint8_t byte;


int printSocketAddress(int fd, char *addrBuffer);

/**
 * It obtains the string representation of an IP address
 * @param address IP address
 * @param buffer buffer to store the address
 * @param bufferSize max size of the buffer
 * @return Pointer to the string
 */
const char * GetIPFromAddress(struct sockaddr_storage *address, char * buffer, size_t bufferSize);

/**
 * It obtains the string representations of a port
 * @param address Address where the port is stored
 * @param buffer buffer to store the port
 * @param bufferSize max size of the buffer
 * @return Pointer to the string
 */
const char *GetPortFromAddress(struct sockaddr_storage *address, char * buffer, size_t bufferSize);

void PrintAccessLog(const char * username, struct sockaddr_storage * src,byte * destAddress,int destAddressType,byte destPort[2],int command);

/**
 * It returns the name of the shutdown mode selected
 * @param shutdownMode
 * @return
 */
const char * GetShutdownModeName(int shutdownMode);

/**
 * It finds out the address family (IPv4 or IPv6)
 * @param address  unknown address
 * @return AF_INET if IPv4, AF_INET6 if IPv6 or AF_UNSPEC if unknown
 */
int GetAddressFamily(const char * address);

/**
 * It converts a port from network order to a single integer
 * @param port 2 byte array
 * @return port number
 */
uint16_t GetPortNumberFromNetworkOrder(const byte port[2]);



#endif //SERVER_UTILS_H
