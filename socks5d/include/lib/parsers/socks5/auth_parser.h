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
    AuthInvalidState,
    AuthFinished
} AuthParserState;


typedef struct {
    AuthParserState State;
    uint8_t ULen;
    int UNamePosition;
    char UName[256];
    uint8_t PLen;
    char Passwd[256];
    int PasswdPosition;
} AuthParser;


/**
 * It creates a new instance of the AuthParser
 * @return a new instance of the AuthParser
 */
//AuthParser AuthParserInit();

/**
 * It safely disposes an instance of the AuthParser
 * @param p Pointer to the AuthParser to dispose
 */
void AuthParserReset(AuthParser *p);

/**
 * It iterates the AuthParser one step for a given input
 * @param p Pointer to an AuthParser
 * @param c Byte to feed the AuthParser
 * @return The CurrentState state
 */
AuthParserState AuthParserFeed(AuthParser *p, byte c);

/**
 * It iterates through the AuthParser for a given number of steps
 * @param p Pointer to the parser instance
 * @param c Array of bytes to feed the parser
 * @param length Total amount of bytes to feed the parser
 * @return the number of bytes consumed
 */
size_t AuthParserConsume(AuthParser *p, byte *c, size_t length);

/**
 * It checks if the parser reached a failed state
 * @param state Current state
 * @return True if the parser is in a failed state
 */
bool AuthParserHasFailed(AuthParserState state);

/**
 * It checks if the parser has reached a final state
 * @param state Current state
 * @return True if the parser is in a final state
 */
bool AuthParserHasFinished(AuthParserState state);


#endif //SERVER_AUTH_PARSER_H
