
#ifndef SERVER_HELLO_PARSER_H
#define SERVER_HELLO_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "utils/utils.h"

//    The client connects to the server, and sends a version
//    identifier/method selection message:

//                    +----+----------+----------+
//                    |VER | NMETHODS | METHODS  |
//                    +----+----------+----------+
//                    | 1  |    1     | 1 to 255 |
//                    +----+----------+----------+

//    The VER field is set to X'05' for this version of the protocol.  The
//    NMETHODS field contains the number of method identifier octets that
//    appear in the METHODS field.




typedef enum {
    HelloVersion,
    HelloNMethods,
    HelloMethods,
    HelloDone,
    HelloErrorUnsupportedVersion,
    HelloInvalidState,
} HelloParserState;

typedef struct {
    HelloParserState State;
    uint8_t NMethods;
    uint8_t CurrentMethod;
    uint8_t Methods[255];
} HelloParser;

/**
 * It initializes a HelloParser instance
 * @return A new HelloParser instance
 */
HelloParser HelloParserInit();

/**
 * It safely disposes a parser instance
 * @param p Parser instance to be disposed
 */
void HelloParserReset(HelloParser *p);

/**
 * It iterates the parser one step for a given input
 * @param p Pointer to an AuthParser
 * @param c Byte to feed the AuthParser
 * @return The CurrentState state
 */
HelloParserState HelloParserFeed(HelloParser *p, byte c);

/**
 * It iterates through the parser for a given number of steps
 * @param p Pointer to the parser instance
 * @param c Array of bytes to feed the parser
 * @param length Total amount of bytes to feed the parser
 * @return the number of bytes consumed
 */
size_t HelloParserConsume(HelloParser *p, byte *c, size_t length);

/**
 * It checks if the parser reached a failed state
 * @param state Current state
 * @return True if the parser is in a failed state
 */
bool HelloParserHasFailed(HelloParserState state);

/**
 * It checks if the parser has reached a final state
 * @param state Current state
 * @return True if the parser is in a final state
 */
bool HelloParserHasFinished(HelloParserState state);


#endif //SERVER_HELLO_PARSER_H
