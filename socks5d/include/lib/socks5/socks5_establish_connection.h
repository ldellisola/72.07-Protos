//
// Created by Lucas Dell'Isola on 14/06/2022.
//

#ifndef SOCKS5D_SOCKS5_ESTABLISH_CONNECTION_H
#define SOCKS5D_SOCKS5_ESTABLISH_CONNECTION_H

#include "utils/buffer.h"

void EstablishConnectionInit(unsigned state, void * data);

void EstablishConnectionClose(unsigned state, void * data);

unsigned EstablishConnectionRun(void * data);

#endif //SOCKS5D_SOCKS5_ESTABLISH_CONNECTION_H
