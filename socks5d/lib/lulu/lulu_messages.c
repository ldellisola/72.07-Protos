//
// Created by tluci on 19/6/2022.
//

#include "lulu/lulu_messages.h"

#include "utils/logger.h"
void fillBuffer(char* string,byte * buffer, int length );

size_t BuildClientHelloResponse(byte *buffer, size_t length, bool authenticationSucceeded) {

    if(authenticationSucceeded){
        if (length < LEN_OK) {
            Error("Buffer to small to WriteHead ClientHelloResponse");
            return 0;
        }
        fillBuffer("+OK\r\n", buffer,LEN_OK );
        return LEN_OK;
    }else{
        if (length < LEN_AUTHENTICATION_ERROR) {
            Error("Buffer to small to WriteHead ClientHelloResponse");
            return 0;
        }
        fillBuffer("-AUTHENTICATION ERROR\r\n", buffer, LEN_AUTHENTICATION_ERROR);
        return LEN_AUTHENTICATION_ERROR;
    }
}

size_t BuildClientGoodbyeResponse(byte *buffer, size_t length) {

    if (length < LEN_GOODBYE) {
        Error("Buffer to small to WriteHead ClientHelloResponse");
        return 0;
    }
    fillBuffer("+GOODBYE\r\n", buffer,LEN_GOODBYE );

    return LEN_GOODBYE;
}

void fillBuffer(char* string,byte * buffer, int length ){
    for (int i = 0; i < length; i++) {
        buffer[i] = string[i];
    }
}
