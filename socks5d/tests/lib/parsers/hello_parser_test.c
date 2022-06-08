//
// Created by Lucas Dell'Isola on 07/06/2022.
//

#include <check.h>
#include <stdio.h>
#include "hello_parser_test.h"
#include "parsers/hello_parser.h"

HelloParser *parser;


/**************************************************************************
 *                      Consume Tests
 ***************************************************************************/

START_TEST(Consumes_WholeMessageWithMultipleAuthenticationMethods_Succeeds)
    {
        // Arrange
        int messageLength = 4;
        byte message[] = {0x05,0x02,0x00,0x01};
        //Act
        int bytesConsumed = HelloParserConsume(parser,message,messageLength);
        //Assert
        ck_assert_int_eq(bytesConsumed, messageLength);
        ck_assert_int_eq(parser->State,HelloDone);
        ck_assert_int_eq(parser->NMethods, 2);
        ck_assert_ptr_ne(parser->Methods,null);
        ck_assert_int_eq(parser->Methods[0], 0x00);
        ck_assert_int_eq(parser->Methods[1], 0x01);
        ck_assert_int_eq(parser->CurrentMethod, 2);
    }
END_TEST

START_TEST(Consumes_WholeMessageWithSingleAuthenticationMethod_Succeeds)
    {
        // Arrange
        int messageLength = 3;
        byte message[] = {0x05,0x01,0x00};
        // Act
        int bytesConsumed = HelloParserConsume(parser,message,messageLength);
        // Assert
        ck_assert_int_eq(bytesConsumed, messageLength);
        ck_assert_int_eq(parser->State,HelloDone);
        ck_assert_int_eq(parser->NMethods, 1);
        ck_assert_ptr_ne(parser->Methods,null);
        ck_assert_int_eq(parser->Methods[0], 0x00);
        ck_assert_int_eq(parser->CurrentMethod, 1);
    }
END_TEST

/**************************************************************************
 *                      Feeds Tests
 ***************************************************************************/

START_TEST(Feeds_CorrectSOCKS5Version_Succeeds)
    {
        // Arrange
        int socks5Version = 0x05;
        // Act
        HelloParserState state = HelloParserFeed(parser,socks5Version);
        // Assert
        ck_assert_int_eq(parser->State, HelloNMethods);
        ck_assert_int_eq(parser->State, state);
    }
END_TEST

START_TEST(Feeds_IncorrectSOCKS5Version_Fails)
    {
        // Arrange
        int socks5Version = 0x04;
        // Act
        HelloParserState state = HelloParserFeed(parser,socks5Version);
        // Assert
        ck_assert_int_eq(parser->State, state);
        ck_assert_int_eq(parser->State, HelloErrorUnsupportedVersion);
    }
END_TEST

START_TEST(Feeds_NullParser_Fails)
    {
        // Arrange
        HelloParser * nullParser = null;
        int socks5Version = 0x05;
        // Act
        HelloParserState state = HelloParserFeed(nullParser,socks5Version);
        // Assert
        ck_assert_int_eq(state, HelloInvalidState);
    }
END_TEST

START_TEST(Feeds_ValidNumberOfMethods_Succeeds){
    // Arrange
    byte nMethods = 2;
    parser->State = HelloNMethods;
    // Act
    HelloParserState state = HelloParserFeed(parser,nMethods);
    // Assert
        ck_assert_int_eq(state,HelloMethods);
        ck_assert_int_eq(parser->NMethods,nMethods);
        ck_assert_ptr_ne(parser->Methods, null);
}
END_TEST

START_TEST(Feeds_InvalidNumberOfMethods_Fails){
    // Arrange
    byte nMethods = 0;
    parser->State = HelloNMethods;
    // Act
    HelloParserState state = HelloParserFeed(parser,nMethods);
    // Assert
        ck_assert_int_eq(state,HelloInvalidState);
        ck_assert_int_eq(parser->NMethods,nMethods);
        ck_assert_ptr_eq(parser->Methods, null);
}
END_TEST

START_TEST(Feeds_SingleMethod_Succeeds)
    {
        // Arrange
        int nMethods = 1;
        uint8_t methods[1];
        parser->Methods = (uint8_t *) &methods;
        parser->NMethods = nMethods;
        parser->State = HelloMethods;
        int method = 0x01;
        // Act
        HelloParserState state = HelloParserFeed(parser,method);
        // Assert
        ck_assert_int_eq(state,HelloDone);
        ck_assert_int_eq(parser->Methods[0],method);
        ck_assert_int_eq(parser->CurrentMethod,parser->NMethods);
        // Clean up
        parser->Methods = null;
    }
    END_TEST

START_TEST(Feeds_MultipleMethod_Succeeds)
    {
        // Arrange
        int nMethods = 2;
        uint8_t methods[2];
        parser->Methods = (uint8_t *) &methods;
        parser->NMethods = nMethods;
        parser->State = HelloMethods;
        int method = 0x01;
        // Act
        HelloParserState state = HelloParserFeed(parser,method);
        // Assert
        ck_assert_int_eq(state,HelloMethods);
        ck_assert_int_eq(parser->Methods[0],method);
        ck_assert_int_ne(parser->CurrentMethod,parser->NMethods);
        // Clean up
        parser->Methods = null;
    }
    END_TEST

START_TEST(parser_initializes_correcty){
    // Arrange
    // Act
    parser = HelloParserInit();
    // Assert
    ck_assert_int_eq(parser->State,HelloVersion);
    ck_assert_ptr_eq(parser->Methods, null);
    // Dispose
    HelloParserDestroy(parser);
}
END_TEST


void SetupHelloParser(void) {
    parser = HelloParserInit();
}

void TeardownHelloParser(void) {
    HelloParserDestroy(parser);
}

Suite *RegisterHelloParserTestSuit() {
    Suite *s = suite_create("HelloParser");

    TCase *tc = tcase_create("HelloParser");
    tcase_add_test(tc, parser_initializes_correcty);
    tcase_add_test(tc, Feeds_NullParser_Fails);
    suite_add_tcase(s, tc);

    TCase *tc2 = tcase_create("HelloParser with TeardownHelloParser");
    tcase_add_checked_fixture(tc2, SetupHelloParser, TeardownHelloParser);
    tcase_add_test(tc2, Consumes_WholeMessageWithMultipleAuthenticationMethods_Succeeds);
    tcase_add_test(tc2, Consumes_WholeMessageWithSingleAuthenticationMethod_Succeeds);
    tcase_add_test(tc2, Feeds_CorrectSOCKS5Version_Succeeds);
    tcase_add_test(tc2, Feeds_IncorrectSOCKS5Version_Fails);
    tcase_add_test(tc2, Feeds_ValidNumberOfMethods_Succeeds);
    tcase_add_test(tc2, Feeds_InvalidNumberOfMethods_Fails);
    tcase_add_test(tc2, Feeds_MultipleMethod_Succeeds);
    tcase_add_test(tc2, Feeds_SingleMethod_Succeeds);
    suite_add_tcase(s, tc2);

    return s;
}
