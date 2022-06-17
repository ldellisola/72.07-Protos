//
// Created by Lucas Dell'Isola on 08/06/2022.
//

#include <string.h>
#include "socks5/socks5_messages.h"
#include "utils/logger.h"
#include "socks5/socks5.h"


size_t BuildHelloResponse(byte *buffer, size_t length, int authenticationMethod) {
    if (length < 2) {
        LogError(false, "Buffer to small to WriteHead HelloResponse");
        return 0;
    }

    buffer[0] = SOCKS5_PROTOCOL_VERSION;
    buffer[1] = authenticationMethod;
    return 2;
}

size_t BuildAuthResponse(byte *buffer, size_t length, bool authenticationSucceeded) {
    if (length < 2) {
        LogError(false, "Buffer to small to WriteHead AuthResponse");
        return 0;
    }

    buffer[0] = SOCKS5_AUTH_NEGOTIATION_VERSION;
    buffer[1] = authenticationSucceeded ? SOCKS5_AUTH_SUCCESS : SOCKS5_AUTH_FAILED;
    return 2;
}

size_t
BuildRequestResponseWithIPv4(byte *buffer, size_t length, int replyCommand, const byte *address, const byte *port) {
    if (length < 10) {
        LogError(false, "Buffer to small to WriteHead RequestResponse with IPv4");
        return 0;
    }
    if (null == address) {
        LogError(false, "Address cannot be null");
        return 0;
    }

    buffer[0] = SOCKS5_PROTOCOL_VERSION;
    buffer[1] = replyCommand;
    buffer[2] = 0;
    buffer[3] = SOCKS5_ADDRESS_TYPE_IPV4;
    buffer[4] = address[0];
    buffer[5] = address[1];
    buffer[6] = address[2];
    buffer[7] = address[3];
    buffer[8] = port[0];
    buffer[9] = port[1];
    return 10;
}

size_t
BuildRequestResponseWithIPv6(byte *buffer, size_t length, int replyCommand, const byte *address, const byte *port) {
    if (length < 22) {
        LogError(false, "Buffer to small to WriteHead RequestResponse with IPv6");
        return 0;
    }
    if (null == address) {
        LogError(false, "Address cannot be null");
        return 0;
    }

    buffer[0] = SOCKS5_PROTOCOL_VERSION;
    buffer[1] = replyCommand;
    buffer[2] = 0;
    buffer[3] = SOCKS5_ADDRESS_TYPE_IPV6;
    for (int i = 0; i < 16; ++i)
        buffer[4 + i] = address[i];
    buffer[20] = port[0];
    buffer[21] = port[1];

    return 22;
}

size_t
BuildRequestResponseWithFQDN(byte *buffer, size_t length, int replyCommand, const char *address, const byte *port) {
    if (null == address) {
        LogError(false, "Address cannot be null");
        return 0;
    }
    size_t addressLen = strlen(address);
    if (length < 5 + addressLen + 1) {
        LogError(false, "Buffer to small to WriteHead RequestResponse with IPv6");
        return 0;
    }

    buffer[0] = SOCKS5_PROTOCOL_VERSION;
    buffer[1] = replyCommand;
    buffer[2] = 0;
    buffer[3] = SOCKS5_ADDRESS_TYPE_FQDN;
    buffer[4] = addressLen;
    for (size_t i = 0; i < addressLen; ++i)
        buffer[5 + i] = address[i];
    buffer[5 + addressLen] = port[0];
    buffer[5 + addressLen + 1] = port[1];

    return addressLen + 2 + 5;
}

size_t
BuildRequestResponse(byte *buffer, size_t length, int replyCommand, int addressType, void *address, const byte *port) {

    switch (addressType) {
        case SOCKS5_ADDRESS_TYPE_FQDN:
            return BuildRequestResponseWithFQDN(buffer, length, replyCommand, address, port);
        case SOCKS5_ADDRESS_TYPE_IPV4:
            return BuildRequestResponseWithIPv4(buffer, length, replyCommand, address, port);
        case SOCKS5_ADDRESS_TYPE_IPV6:
            return BuildRequestResponseWithIPv6(buffer, length, replyCommand, address, port);
        default:
            LogError(false, "Unsupported Address Type: %d", addressType);
            return -1;
    }
}

size_t BuildRequestResponseFromParser(byte *buffer, size_t length, int replyCommand, RequestParser *parser) {
    if (null == parser) {
        LogError(false, "RequestParser cannot be null");
        return -1;
    }
    return BuildRequestResponse(buffer, length, replyCommand, parser->AType, parser->DestAddress, parser->DestPort);
}
















