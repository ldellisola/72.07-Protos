//
// Created by Lucas Dell'Isola on 30/05/2022.
//

#ifndef SERVER_SOCKS_SERVER_H
#define SERVER_SOCKS_SERVER_H
#include <stdbool.h>

/**
 * It initializes a Socks5Server on IPv4
 * @param port Port where the server will be listening to, in the form of a null terminated string
 * @return Whether the server was started successfully
 */
bool RegisterSocks5ServerOnIPv4(const char *port, const char *address);

/**
 * It initializes a Socks5Server on IPv6
 * @param port Port where the server will be listening to, in the form of a null terminated string
 * @return Whether the server was started successfully
 */
bool RegisterSocks5ServerOnIPv6(const char *port, const char *address);

/**
 * It initialized a Socks5Server. If an address value is supplied, it will start only on that interface.
 * If no address value is provided (null), then it will listen on all interfaces
 * @param port Port where the server will be listening to, in the form of a null terminated string
 * @param address Address to bind the server to. It can be null
 * @return Whether the server was started successfully
 */
bool RegisterSocks5Server(const char *port, const char *address, int poolSize);


/**
 * It safely disposes all the allocated memory by the socks5 server
 */
void DisposeSocks5Server();


#endif //SERVER_SOCKS_SERVER_H
