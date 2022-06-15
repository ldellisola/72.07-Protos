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


#endif //SERVER_UTILS_H
