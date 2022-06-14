//
// Created by Lucas Dell'Isola on 08/06/2022.
//

#include "request_parser_test.h"
#include "parsers/request_parser.h"
RequestParser parser;

/**************************************************************************
 *                      Init Tests
 ***************************************************************************/

START_TEST(Init_Succeeds){
    // Arrange
    // Act
    parser = RequestParserInit();
    // Assert
    ck_assert_int_eq(parser.State, RequestVersion);
    ck_assert_int_eq(parser.AddressLength,0);
    ck_assert_int_eq(parser.AddressPosition,0);
    ck_assert_int_eq(parser.CMD,0);
    ck_assert_int_eq(parser.AType,0);
    for (int i = 0; i < 2; ++i)
        ck_assert_int_eq(parser.DestPort[i],0);
    for (int i = 0; i < 256; ++i)
        ck_assert_int_eq(parser.DestAddress[i],0);

}
END_TEST

/**************************************************************************
 *                            Feed Tests
 ***************************************************************************/

START_TEST(Feed_NullParser_Fails){
    // Arrange
    RequestParser * nullParser = null;
    byte data = 0x05;
    // Act
    RequestParserState state = RequestParserFeed(nullParser, data);
    // Assert
    ck_assert_int_eq(state,RequestInvalidState);
}
END_TEST

START_TEST(Feed_ValidSocksVersion_Succeeds){
    // Arrange
    byte socks5Version = 0x05;
    // Act
    RequestParserState state = RequestParserFeed(&parser,socks5Version);
    // Assert
        ck_assert_int_eq(state, RequestCMD);

}
END_TEST

START_TEST(Feed_InvalidSocksVersion_Fails){
    // Arrange
    byte socks5Version = 0x03;
    // Act
    RequestParserState state = RequestParserFeed(&parser,socks5Version);
    // Assert
        ck_assert_int_eq(state, RequestErrorUnsupportedVersion);

}
END_TEST

START_TEST(Feed_Command_Succeeds){
    // Arrange
    byte cmd = 0x01;
    parser.State = RequestCMD;
    // Act
    RequestParserState state = RequestParserFeed(&parser,cmd);
    // Assert
        ck_assert_int_eq(state,RequestRSV);
        ck_assert_int_eq(parser.CMD, cmd);
}
END_TEST

START_TEST(Feed_ValidReservedValue_Succeeds){
    // Arrange
    byte rsv = 0;
    parser.State = RequestRSV;
    // Act
    RequestParserState state = RequestParserFeed(&parser,rsv);
        ck_assert_int_eq(state,RequestAType);
}
END_TEST

START_TEST(Feed_InvalidReservedValue_Fails){
    // Arrange
    byte rsv = 1;
    parser.State = RequestRSV;
    // Act
    RequestParserState state = RequestParserFeed(&parser,rsv);
        ck_assert_int_eq(state,RequestInvalidState);
}
END_TEST

START_TEST(Feed_IPV4AddressType_Succeeds){
    // Arrange
    byte atype = 0x01;
    parser.State = RequestAType;
    // Act
    RequestParserState state = RequestParserFeed(&parser,atype);
    // Assert
        ck_assert_int_eq(state,RequestDestAddrIPV4);
        ck_assert_int_eq(parser.AddressLength, 4);
}
END_TEST

START_TEST(Feed_IPV6AddressType_Succeeds){
    // Arrange
    byte atype = 0x04;
    parser.State = RequestAType;
    // Act
    RequestParserState state = RequestParserFeed(&parser,atype);
    // Assert
        ck_assert_int_eq(state,RequestDestAddrIPV6);
        ck_assert_int_eq(parser.AddressLength, 16);
}
END_TEST

START_TEST(Feed_FQDNAddressType_Succeeds){
    // Arrange
    byte atype = 0x03;
    parser.State = RequestAType;
    // Act
    RequestParserState state = RequestParserFeed(&parser,atype);
    // Assert
    ck_assert_int_eq(state,RequestDestAddrFQDN);
}
END_TEST

START_TEST(Feed_InvalidAddressType_Fails){
    // Arrange
    byte atype = 0x99;
    parser.State = RequestAType;
    // Act
    RequestParserState state = RequestParserFeed(&parser,atype);
    // Assert
    ck_assert_int_eq(state,RequestInvalidState);
}
END_TEST

START_TEST(Feed_ValidDestAddressFQDNSize_Succeeds){
    // Arrange
    byte size = 15;
    parser.State = RequestDestAddrFQDN;
    // Act
    RequestParserState state = RequestParserFeed(&parser,size);
    // Assert
    ck_assert_int_eq(state, RequestDestAddrFQDN);
    ck_assert_int_eq(parser.AddressLength,size);
}
END_TEST

START_TEST(Feed_InvalidDestAddressFQDNSize_Fails){
    // Arrange
    byte size = 0;
    parser.State = RequestDestAddrFQDN;
    // Act
    RequestParserState state = RequestParserFeed(&parser,size);
    // Assert
    ck_assert_int_eq(state, RequestInvalidState);
}
END_TEST

START_TEST(Feed_DestAddressFQDNIncompleteContent_Succeeds){
        // Arrange
        parser.AddressLength = 15;
        byte letter = 'L';
        parser.State = RequestDestAddrFQDN;
        // Act
        RequestParserState state = RequestParserFeed(&parser,letter);
        // Assert
        ck_assert_int_eq(state, RequestDestAddrFQDN);
        ck_assert_ptr_ne(parser.DestAddress, null);
        ck_assert_int_eq(parser.DestAddress[0],letter);
        ck_assert_int_ne(parser.AddressPosition,parser.AddressLength);
    }
END_TEST

START_TEST(Feed_DestAddressFQDNCompleteContent_Succeeds){
        // Arrange
        parser.AddressLength = 1;
        byte letter = 'L';
        parser.State = RequestDestAddrFQDN;
        // Act
        RequestParserState state = RequestParserFeed(&parser,letter);
        // Assert
        ck_assert_int_eq(state, RequestDestPortFirstByte);
        ck_assert_ptr_ne(parser.DestAddress, null);
        ck_assert_int_eq(parser.DestAddress[0],letter);
        ck_assert_int_eq(parser.AddressPosition,parser.AddressLength);
    }
END_TEST

START_TEST(Feed_DestAddressIPV4IncompleteContent_Succeeds){
        // Arrange
        byte letter = 127;
        parser.State = RequestDestAddrIPV4;
        // Act
        RequestParserState state = RequestParserFeed(&parser,letter);
        // Assert
        ck_assert_int_eq(state, RequestDestAddrIPV4);
        ck_assert_ptr_ne(parser.DestAddress, null);
        ck_assert_int_eq(parser.DestAddress[0],letter);
        ck_assert_int_ne(parser.AddressPosition,parser.AddressLength);
    }
END_TEST

START_TEST(Feed_DestAddressIPV4CompleteContent_Succeeds){
        // Arrange
        byte letter = 1;
        parser.DestAddress[0] = 127;
        parser.DestAddress[1] = 0;
        parser.DestAddress[2] = 0;
        parser.AddressLength =4;
        parser.AddressPosition =3;
        parser.State = RequestDestAddrIPV4;
        // Act
        RequestParserState state = RequestParserFeed(&parser,letter);
        // Assert
        ck_assert_int_eq(state, RequestDestPortFirstByte);
        ck_assert_int_eq(parser.DestAddress[3],letter);
        ck_assert_int_eq(parser.AddressPosition,parser.AddressLength);
    }
END_TEST

START_TEST(Feed_DestAddressIPV6IncompleteContent_Succeeds){
        // Arrange
        byte letter = 127;
        parser.State = RequestDestAddrIPV6;
        // Act
        RequestParserState state = RequestParserFeed(&parser,letter);
        // Assert
        ck_assert_int_eq(state, RequestDestAddrIPV6);
        ck_assert_ptr_ne(parser.DestAddress, null);
        ck_assert_int_eq(parser.DestAddress[0],letter);
        ck_assert_int_ne(parser.AddressPosition,parser.AddressLength);
    }
END_TEST

START_TEST(Feed_DestAddressIPV6CompleteContent_Succeeds){
        // Arrange
        byte letter = 2;
        parser.AddressLength = 16;
        parser.AddressPosition = 15;
        for(int i = 0 ; i < parser.AddressPosition ; i++)
            parser.DestAddress[i] = 1;

        parser.State = RequestDestAddrIPV6;
        // Act
        RequestParserState state = RequestParserFeed(&parser,letter);
        // Assert
        ck_assert_int_eq(state, RequestDestPortFirstByte);
        ck_assert_int_eq(parser.DestAddress[15],letter);
        ck_assert_int_eq(parser.AddressPosition,parser.AddressLength);
    }
END_TEST

START_TEST(Feed_PortFirstByte_Succeeds){
    // Arrange
    parser.State = RequestDestPortFirstByte;
    byte content = 0x01;
    // Act
    RequestParserState state = RequestParserFeed(&parser,content);
    // Assert
        ck_assert_int_eq(state,RequestDestPortSecondByte);
        ck_assert_int_eq(parser.DestPort[0], content);
}
END_TEST

START_TEST(Feed_PortSecondByte_Succeeds){
    // Arrange
    parser.State = RequestDestPortSecondByte;
    byte content = 0x02;
    // Act
    RequestParserState state = RequestParserFeed(&parser,content);
    // Assert
        ck_assert_int_eq(state,RequestDone);
        ck_assert_int_eq(parser.DestPort[1],0x02 );
}
END_TEST

/**************************************************************************
 *                            Feed Tests
 ***************************************************************************/

START_TEST(Consume_CompleteMessageWithIPV4_Succeeds){
    // Arrange
    byte socksVersion = 0x05;
    byte cmd = 0x01;
    byte atype = 0x01;
    byte address[] = {127,0,0,1};
    byte port[] = {0x01,0x02};
    int messageLength = 10;
    byte message[] = {
            socksVersion,cmd,0,atype,
            address[0], address[1],address[2], address[3],
            port[0],port[1]
    };
    // Act
    int bytes = RequestParserConsume(&parser,message,messageLength);

    // Assert
        ck_assert_int_eq(bytes,messageLength);
        ck_assert_int_eq(parser.State,RequestDone);
        ck_assert_int_eq(parser.CMD,cmd);
        ck_assert_int_eq(parser.AddressLength,4);
        ck_assert_int_eq(parser.DestAddress[0],address[0]);
        ck_assert_int_eq(parser.DestAddress[1],address[1]);
        ck_assert_int_eq(parser.DestAddress[2],address[2]);
        ck_assert_int_eq(parser.DestAddress[3],address[3]);
        ck_assert_int_eq(parser.DestPort[0],port[0]);
        ck_assert_int_eq(parser.DestPort[1], port[1]);
}
END_TEST

START_TEST(Consume_CompleteMessageWithIPV6_Succeeds){
    // Arrange
    byte socksVersion = 0x05;
    byte cmd = 0x01;
    byte atype = 0x04;
    byte address[] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
    byte port[] = {0x01,0x02};
    int messageLength = 22;
    byte message[] = {
            socksVersion,cmd,0,atype,
            address[0], address[1],address[2], address[3],
            address[4], address[5],address[6], address[7],
            address[8], address[9],address[10], address[11],
            address[12], address[13],address[14], address[15],
            port[0],port[1]
    };
    // Act
    int bytes = RequestParserConsume(&parser,message,messageLength);

    // Assert
        ck_assert_int_eq(bytes,messageLength);
        ck_assert_int_eq(parser.State,RequestDone);
        ck_assert_int_eq(parser.CMD,cmd);
        ck_assert_int_eq(parser.AddressLength,16);
        for (int i = 0; i < 16; ++i)
            ck_assert_int_eq(parser.DestAddress[i],address[i]);
        ck_assert_int_eq(parser.DestPort[0],port[0]);
        ck_assert_int_eq(parser.DestPort[1], port[1]);
}
END_TEST


START_TEST(Consume_CompleteMessageWithFQDN_Succeeds){
        // Arrange
        byte socksVersion = 0x05;
        byte cmd = 0x01;
        byte atype = 0x03;
        byte addrLen = 10;
        char * address = "protos.com";
        byte port[] = {0x01,0x02};
        int messageLength = 17;
        byte message[] = {
                socksVersion,cmd,0,atype,addrLen,
                address[0], address[1],address[2], address[3],
                address[4], address[5],address[6], address[7],
                address[8], address[9],port[0],port[1]
        };
        // Act
        int bytes = RequestParserConsume(&parser,message,messageLength);

        // Assert
        ck_assert_int_eq(bytes,messageLength);
        ck_assert_int_eq(parser.State,RequestDone);
        ck_assert_int_eq(parser.CMD,cmd);
        ck_assert_int_eq(parser.AddressLength,addrLen);
        ck_assert_str_eq((char*)parser.DestAddress,address);
        ck_assert_int_eq(parser.DestPort[0],port[0]);
        ck_assert_int_eq(parser.DestPort[1], port[1]);
    }
END_TEST


START_TEST(Consume_IncompleteMessage_Succeeds){
        // Arrange
        byte socksVersion = 0x05;
        byte cmd = 0x01;
        byte atype = 0x03;
        byte addrLen = 10;
        int messageLength = 5;
        byte message[] = {
                socksVersion,cmd,0,atype,addrLen
        };
        // Act
        int bytes = RequestParserConsume(&parser,message,messageLength);
        // Assert
        ck_assert_int_eq(bytes,messageLength);
        ck_assert_int_eq(parser.State,RequestDestAddrFQDN);
        ck_assert_int_eq(parser.CMD,cmd);
        ck_assert_int_eq(parser.AddressLength,addrLen);
    }
END_TEST


START_TEST(Consume_CompleteMessageWithExtraLength_Succeeds){
        // Arrange
        byte socksVersion = 0x05;
        byte cmd = 0x01;
        byte atype = 0x03;
        byte addrLen = 10;
        char * address = "protos.com";
        byte port[] = {0x01,0x02};
        int messageLength = 22;
        byte message[] = {
                socksVersion,cmd,0,atype,addrLen,
                address[0], address[1],address[2], address[3],
                address[4], address[5],address[6], address[7],
                address[8], address[9],port[0],port[1],
                0,0,0,0,0
        };
        // Act
        int bytes = RequestParserConsume(&parser,message,messageLength);

        // Assert
        ck_assert_int_eq(bytes,17);
        ck_assert_int_eq(parser.State,RequestDone);
        ck_assert_int_eq(parser.CMD,cmd);
        ck_assert_int_eq(parser.AddressLength,addrLen);
        ck_assert_str_eq((char*)parser.DestAddress,address);
        ck_assert_int_eq(parser.DestPort[0],port[0]);
        ck_assert_int_eq(parser.DestPort[1], port[1]);
    }
END_TEST


void SetupRequestParser(void) {
    parser = RequestParserInit();
}

void TeardownRequestParser(void) {
    RequestParserReset(&parser);
}

Suite *RegisterRequestParserTestSuit() {
    Suite *s = suite_create("RequestParser");

    TCase *tc = tcase_create("RequestParser");
    tcase_add_test(tc, Init_Succeeds);
    tcase_add_test(tc, Feed_NullParser_Fails);
    suite_add_tcase(s, tc);

    TCase *tc2 = tcase_create("RequestParser with TeardownRequestParser");
    tcase_add_checked_fixture(tc2, SetupRequestParser, TeardownRequestParser);
    tcase_add_test(tc2, Feed_ValidSocksVersion_Succeeds);
    tcase_add_test(tc2, Feed_InvalidSocksVersion_Fails);
    tcase_add_test(tc2, Feed_Command_Succeeds);
    tcase_add_test(tc2, Feed_ValidReservedValue_Succeeds);
    tcase_add_test(tc2, Feed_InvalidReservedValue_Fails);
    tcase_add_test(tc2, Feed_IPV4AddressType_Succeeds);
    tcase_add_test(tc2, Feed_IPV6AddressType_Succeeds);
    tcase_add_test(tc2, Feed_FQDNAddressType_Succeeds);
    tcase_add_test(tc2, Feed_InvalidAddressType_Fails);
    tcase_add_test(tc2, Feed_ValidDestAddressFQDNSize_Succeeds);
    tcase_add_test(tc2, Feed_InvalidDestAddressFQDNSize_Fails);
    tcase_add_test(tc2, Feed_DestAddressFQDNIncompleteContent_Succeeds);
    tcase_add_test(tc2, Feed_DestAddressFQDNCompleteContent_Succeeds);
    tcase_add_test(tc2, Feed_DestAddressIPV4IncompleteContent_Succeeds);
    tcase_add_test(tc2, Feed_DestAddressIPV4CompleteContent_Succeeds);
    tcase_add_test(tc2, Feed_DestAddressIPV6IncompleteContent_Succeeds);
    tcase_add_test(tc2, Feed_DestAddressIPV6CompleteContent_Succeeds);
    tcase_add_test(tc2, Feed_PortFirstByte_Succeeds);
    tcase_add_test(tc2, Feed_PortSecondByte_Succeeds);
    tcase_add_test(tc2, Consume_CompleteMessageWithIPV4_Succeeds);
    tcase_add_test(tc2, Consume_CompleteMessageWithIPV6_Succeeds);
    tcase_add_test(tc2, Consume_CompleteMessageWithFQDN_Succeeds);
    tcase_add_test(tc2, Consume_IncompleteMessage_Succeeds);
    tcase_add_test(tc2, Consume_CompleteMessageWithExtraLength_Succeeds);

    suite_add_tcase(s, tc2);

    return s;
}
