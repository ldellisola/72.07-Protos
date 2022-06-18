//
// Created by Lucas Dell'Isola on 14/06/2022.
//

#ifndef SOCKS5D_SOCKS5_CONNECTED_H
#define SOCKS5D_SOCKS5_CONNECTED_H


void ConnectedConnectionInit(unsigned int state, void *data);
void ConnectedConnectionClose(unsigned int state, void *data);

unsigned ConnectedConnectionRun(void *data);


#endif //SOCKS5D_SOCKS5_CONNECTED_H
