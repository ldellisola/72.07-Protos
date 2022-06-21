

#ifndef SERVER_REQUEST_PARSER_H
#define SERVER_REQUEST_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "utils/utils.h"
#include <netinet/in.h>
#include "socks5/socks5.h"

//Once the method-dependent subnegotiation has completed, the client
//        sends the request details.  If the negotiated method includes
//        encapsulation for purposes of integrity checking and/or
//        confidentiality, these requests MUST be encapsulated in the method-
//dependent encapsulation.
//
//The SOCKS request is formed as follows:
//
//+----+-----+-------+------+----------+----------+
//|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
//+----+-----+-------+------+----------+----------+
//| 1  |  1  | X'00' |  1   | Variable |    2     |
//+----+-----+-------+------+----------+----------+
//
//Where:
//
//o  VER    protocol version: X'05'
//o  CMD
//  o  CONNECT X'01'
//  o  BIND X'02'
//  o  UDP ASSOCIATE X'03'
//o  RSV    RESERVED
//o  ATYP   address type of following address
//  o  IP V4 address: X'01'
//  o  DOMAINNAME: X'03'
//  o  IP V6 address: X'04'
//o  DST.ADDR       desired destination address
//o  DST.PORT desired destination port in network octet
//        order
//
//The SOCKS server will typically evaluate the request based on source
//        and destination addresses, and return one or more reply messages, as
//        appropriate for the request type.
//5.  Addressing
//
//   In an address field (DST.ADDR, BND.ADDR), the ATYP field specifies
//   the type of address contained within the field:
//
//          o  X'01'
//
//   the address is a version-4 IP address, with a length of 4 octets
//
//          o  X'03'
//
//   the address field contains a fully-qualified domain name.  The first
//   octet of the address field contains the number of octets of name that
//   follow, there is no terminating NUL octet.
//
//          o  X'04'
//
//   the address is a version-6 IP address, with a length of 16 octets.



typedef enum {
    RequestVersion,
    RequestCMD,
    RequestRSV,
    RequestAType,
    RequestDestAddrIPV4,
    RequestDestAddrIPV6,
    RequestDestAddrFQDN,
    RequestDestPortFirstByte,
    RequestDestPortSecondByte,
    RequestErrorUnsupportedVersion,
    RequestInvalidState,
    RequestDone,
} RequestParserState;


typedef struct {
    RequestParserState State;
    byte CMD;
    byte AType;
    byte DestAddress[256];
    byte AddressLength;
    byte AddressPosition;
    byte DestPort[2];
} RequestParser;

/**
 * It restores a RequestParser instance
 * @param p The parser instance
 */
void RequestParserReset(RequestParser *p);

/**
 * It iterates the parser one step for a given input
 * @param p Pointer to an AuthParser
 * @param c Byte to feed the AuthParser
 * @return The CurrentState state
 */
RequestParserState RequestParserFeed(RequestParser *p, byte c);

/**
 * It iterates through the parser for a given number of steps
 * @param p Pointer to the parser instance
 * @param c Array of bytes to feed the parser
 * @param length Total amount of bytes to feed the parser
 * @return number of bytes consumed
 */
size_t RequestParserConsume(RequestParser *p, byte *c, size_t length);

/**
 * It checks if the parser reached a failed state
 * @param state Current state
 * @return True if the parser is in a failed state
 */
bool RequestParserFailed(RequestParserState state);

/**
 * It checks if the parser has reached a final state
 * @param state Current state
 * @return True if the parser is in a final state
 */
bool RequestParserHasFinished(RequestParserState state);

#endif //SERVER_REQUEST_PARSER_H
