

#ifndef SERVER_REQUEST_PARSER_H
#define SERVER_REQUEST_PARSER_H
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "utils/utils.h"
#include <netinet/in.h>

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

static const uint8_t CMD_CONNECT = 0x01;
static const uint8_t CMD_BIND = 0x02;
static const uint8_t ATYP_IPV4 = 0x01;
static const uint8_t ATYP_DOMAINNAME = 0x03;
static const uint8_t ATYP_IPV6 = 0x04;
#define MAX_LENGTH 253


typedef enum
{
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
} RequestParserStates;

typedef union {
    uint8_t First;
    uint8_t Second;
    in_port_t Complete;
} Port;

typedef struct
{
    RequestParserStates State;
    uint8_t CMD;
    uint8_t AType;
    uint8_t * DestAddress;
    uint8_t AddressLength;
    uint8_t AddressPosition;
    Port DestPort;
}RequestParser;


RequestParser * RequestParserInit();
void RequestParserDestroy(RequestParser * p);
bool RequestParserFeed(RequestParser* p, byte c);
bool RequestParserConsume(RequestParser* p, byte* c, int length);
bool RequestParserFinished(RequestParser* p, bool* hasError);

#endif //SERVER_REQUEST_PARSER_H
