//
// Created by Lucas Dell'Isola on 28/05/2022.
//
#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <stdint.h>
#include <stdlib.h>

#define null NULL
#define OK 0
#define ERROR -1


typedef uint8_t byte;


int printSocketAddress(int fd, char *addrBuffer);

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


#endif //SERVER_UTILS_H
