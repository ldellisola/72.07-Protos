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


/**
 * It creates a new instance of the AuthParser
 * @return a new instance of the AuthParser
 */
AuthParser * AuthParserInit();

/**
 * It safely disposes an instance of the AuthParser
 * @param p Pointer to the AuthParser to dispose
 */
void AuthParserDestroy(AuthParser *p);

/**
 * It iterates the AuthParser one step for a given input
 * @param p Pointer to an AuthParser
 * @param c Byte to feed the AuthParser
 * @return True if the parser reached a final state.
 */
bool AuthParserFeed(AuthParser *p, byte c);

/**
 * It iterates through the AuthParser for a given number of steps
 * @param p Pointer to the parser instance
 * @param c Array of bytes to feed the parser
 * @param length Total amount of bytes to feed the parser
 * @return True if the parser reached a final state
 */
bool AuthParserConsume(AuthParser* p, byte * c, int length);

/**
 * It checks if the parser reached a failed state
 * @param p Pointer to the parser instance
 * @return True if the parser is in a failed state
 */
bool AuthParserFailed(AuthParser *p);


#endif //SERVER_AUTH_PARSER_H
