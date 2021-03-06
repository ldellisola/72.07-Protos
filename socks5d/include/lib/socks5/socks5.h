//
// Created by Lucas Dell'Isola on 08/06/2022.
//

#ifndef SOCKS5D_SOCKS5_H
#define SOCKS5D_SOCKS5_H

#define SOCKS5_PROTOCOL_VERSION 0x05
#define SOCKS5_AUTH_NEGOTIATION_VERSION 0x01

#define SOCKS5_AUTH_SUCCESS 0x00
#define SOCKS5_AUTH_FAILED 0xFF

#define SOCKS5_REPLY_NOT_DECIDED (-1)
#define SOCKS5_REPLY_SUCCEEDED 0x00
#define SOCKS5_REPLY_GENERAL_FAILURE 0x01
#define SOCKS5_REPLY_CONNECTION_NOT_ALLOWED  0x02
#define SOCKS5_REPLY_UNREACHABLE_NETWORK 0x03
#define SOCKS5_REPLY_UNREACHABLE_HOST 0x04
#define SOCKS5_REPLY_CONNECTION_REFUSED 0x05
#define SOCKS5_REPLY_TTL_EXPIRED 0x06
#define SOCKS5_REPLY_COMMAND_NOT_SUPPORTED 0x07


#define SOCKS5_CMD_CONNECT  0x01

#define SOCKS5_ADDRESS_TYPE_IPV4  0x01
#define SOCKS5_ADDRESS_TYPE_FQDN  0x03
#define SOCKS5_ADDRESS_TYPE_IPV6  0x04

#define ATTACHMENT(key) ( (Socks5Connection*)((SelectorKey*)(key))->Data)



#endif //SOCKS5D_SOCKS5_H
