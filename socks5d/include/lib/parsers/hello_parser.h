
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

HelloParser * HelloParserInit();

void HelloParserDestroy(HelloParser* p);

bool HelloParserFeed(HelloParser *p, uint8_t b);

bool HelloParserConsume(HelloParser * p, byte * c, int length);

bool HelloParserFinished(HelloParser * p, bool *hasError);


#endif //SERVER_HELLO_PARSER_H
