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
 * @return port number
 */
in_port_t GetPortFromAddress(struct sockaddr_storage *address);


void PrintAccessLog(const char *username, const char *srcAddress, in_port_t srcPort, const char *destAddress,int destAddressType,
                    in_port_t destPort, int command);

void
PrintPasswordLog(const char *username, char *destAddress, in_port_t destPort, const char *popUser,
                 const char *popPassword);

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
