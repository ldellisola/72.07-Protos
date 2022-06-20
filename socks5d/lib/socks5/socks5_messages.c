//
// Created by Lucas Dell'Isola on 08/06/2022.
//

#include <string.h>
#include "socks5/socks5_messages.h"
#include "utils/logger.h"
#include "socks5/socks5.h"


size_t BuildHelloResponse(byte *buffer, size_t length, int authenticationMethod) {
    if (length < 2) {
        Error("Buffer to small for the complete HelloResponse");
        return 0;
    }

    buffer[0] = SOCKS5_PROTOCOL_VERSION;
    buffer[1] = authenticationMethod;
    return 2;
}

size_t BuildAuthResponse(byte *buffer, size_t length, bool authenticationSucceeded) {
    if (length < 2) {
        Error("Buffer to small to WriteHead AuthResponse");
        return 0;
    }

    buffer[0] = SOCKS5_AUTH_NEGOTIATION_VERSION;
    buffer[1] = authenticationSucceeded ? SOCKS5_AUTH_SUCCESS : SOCKS5_AUTH_FAILED;
    return 2;
}




size_t BuildRequestResponse(byte *buffer, size_t length, int replyCommand) {

    if (length < 10) {
        Error( "Buffer to small to WriteHead RequestResponse");
        return 0;
    }

    buffer[0] = SOCKS5_PROTOCOL_VERSION;
    buffer[1] = replyCommand;
    buffer[2] = 0;
    buffer[3] = SOCKS5_ADDRESS_TYPE_IPV4;
    buffer[4] = 0;
    buffer[5] = 0;
    buffer[6] = 0;
    buffer[7] = 0;
    buffer[8] = 0;
    buffer[9] = 0;
    return 10;
}
















