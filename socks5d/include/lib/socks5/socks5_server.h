//
// Created by Lucas Dell'Isola on 30/05/2022.
//

#ifndef SERVER_SOCKS_SERVER_H
#define SERVER_SOCKS_SERVER_H


/**
 * It initializes a Socks5Server
 * @param port Port where the server will be listening to, in the form of a null terminated string
 * @return A TCP ClientTcpConnection
 */
void RegisterSocks5ServerOnIPv4(const char * port);



#endif //SERVER_SOCKS_SERVER_H
