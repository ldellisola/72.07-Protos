
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




typedef enum
{
    HelloVersion,
    HelloNMethods,
    HelloMethods,
    HelloDone,
    HelloErrorUnsupportedVersion,
    HelloInvalidState,
}HelloParserState;

typedef struct
{
    HelloParserState State;
    uint8_t NMethods;
    uint8_t RemainingMethods;
    uint8_t * Methods;
}HelloParser;

/**
 * It initializes a HelloParser instance
 * @return A new HelloParser instance
 */
HelloParser * HelloParserInit();

/**
 * It safely disposes a parser instance
 * @param p Parser instance to be disposed
 */
void HelloParserDestroy(HelloParser* p);

/**
 * It iterates the parser one step for a given input
 * @param p Pointer to an AuthParser
 * @param c Byte to feed the AuthParser
 * @return True if the parser reached a final state.
 */
bool HelloParserFeed(HelloParser *p, byte c);

/**
 * It iterates through the parser for a given number of steps
 * @param p Pointer to the parser instance
 * @param c Array of bytes to feed the parser
 * @param length Total amount of bytes to feed the parser
 * @return True if the parser reached a final state
 */
bool HelloParserConsume(HelloParser * p, byte * c, int length);

/**
 * It checks if the parser reached a failed state
 * @param p Pointer to the parser instance
 * @return True if the parser is in a failed state
 */
bool HelloParserFailed(HelloParser * p);


#endif //SERVER_HELLO_PARSER_H
