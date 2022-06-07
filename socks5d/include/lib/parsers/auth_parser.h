//
// Created by Lucas Dell'Isola on 05/06/2022.
//

#ifndef SERVER_AUTH_PARSER_H
#define SERVER_AUTH_PARSER_H

#include <stdbool.h>
#include "utils/utils.h"

/*
 * Once the SOCKS V5 server has started, and the client has selected the
   Username/Password Authentication protocol, the Username/Password
   subnegotiation begins.  This begins with the client producing a
   Username/Password request:
                +----+------+----------+------+----------+
                |VER | ULEN |  UNAME   | PLEN |  PASSWD  |
                +----+------+----------+------+----------+
                | 1  |  1   | 1 to 255 |  1   | 1 to 255 |
                +----+------+----------+------+----------+
 */

typedef enum {
    AuthVersion,
    AuthULen,
    AuthUName,
    AuthPLen,
    AuthPasswd,
    AuthInvalidProtocol,
    AuthFinished
}AuthParserStates;


typedef struct {
    AuthParserStates State;
    uint8_t ULen;
    int UNamePosition;
    char * UName;
    uint8_t PLen;
    char *Passwd;
    int PasswdPosition;
}AuthParser;


AuthParser * AuthParserInit();

void AuthParserDestroy(AuthParser *p);

bool AuthParserFeed(AuthParser *p, byte c);

bool AuthParserConsume(AuthParser* p, byte * c, int length);

bool AuthParserFailed(AuthParser *p);


#endif //SERVER_AUTH_PARSER_H
