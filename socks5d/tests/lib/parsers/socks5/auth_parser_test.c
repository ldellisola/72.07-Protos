//
// Created by Lucas Dell'Isola on 07/06/2022.
//

#include "auth_parser_test.h"
#include "parsers/socks5/auth_parser.h"

AuthParser parser;

/**************************************************************************
 *                      AuthParserInit Tests
 ***************************************************************************/

START_TEST(Init_Succeeds)
    {
        // Arrange
        // Act
        AuthParserReset(&parser);
        // Assert
        for (int i = 0; i < 256; ++i) {
            ck_assert_int_eq(parser.Passwd[i], 0);
            ck_assert_int_eq(parser.UName[i], 0);
        }

        ck_assert_int_eq(parser.State, AuthVersion);
        ck_assert_int_eq(parser.UNamePosition, 0);
        ck_assert_int_eq(parser.PLen, 0);
        ck_assert_int_eq(parser.PasswdPosition, 0);
        ck_assert_int_eq(parser.ULen, 0);
    }
END_TEST

/**************************************************************************
 *                      AuthParserFeed Tests
 ***************************************************************************/

START_TEST(Feed_NullParser_Fails)
    {
        // Arrange
        AuthParser *nullParser = null;
        // Act
        AuthParserState state = AuthParserFeed(nullParser, 0);
        // Assert
        ck_assert_int_eq(state, AuthInvalidState);
    }
END_TEST

START_TEST(Feed_CorrectSocksVersion_Succeeds)
    {
        // Arrange
        byte socksVersion = 0x01;
        // Act
        AuthParserState state = AuthParserFeed(&parser, socksVersion);
        // Assert
        ck_assert_int_eq(state, AuthULen);
    }
END_TEST

START_TEST(Feed_IncorrectSocksVersion_Fails)
    {
        // Arrange
        byte socksVersion = 0x04;
        // Act
        AuthParserState state = AuthParserFeed(&parser, socksVersion);
        // Assert
        ck_assert_int_eq(state, AuthInvalidProtocol);
    }
END_TEST

START_TEST(Feed_ValidULen_Succeeds)
    {
        // Arrange
        parser.State = AuthULen;
        byte uLen = 10;
        // Act
        AuthParserState state = AuthParserFeed(&parser, uLen);
        // Assert
        ck_assert_int_eq(state, AuthUName);
        ck_assert_int_eq(parser.ULen, uLen);
    }
END_TEST

START_TEST(Feed_InvalidULen_Fails)
    {
        // Arrange
        parser.State = AuthULen;
        byte uLen = 0;
        // Act
        AuthParserState state = AuthParserFeed(&parser, uLen);
        // Assert
        ck_assert_int_eq(state, AuthInvalidState);
    }
END_TEST

START_TEST(Feed_AuthUNameIncomplete_Succeeds)
    {
        // Arrange
        parser.State = AuthUName;
        parser.ULen = 2;
        byte letter = 'L';
        // Act
        AuthParserState state = AuthParserFeed(&parser, letter);
        // Assert
        ck_assert_int_eq(state, AuthUName);
        ck_assert_int_eq(parser.UName[0], letter);
        ck_assert_int_ne(parser.UNamePosition, parser.ULen);
    }
END_TEST

START_TEST(Feed_AuthUNameComplete_Succeeds)
    {
        // Arrange
        parser.State = AuthUName;
        parser.ULen = 1;
        byte letter = 'L';
        // Act
        AuthParserState state = AuthParserFeed(&parser, letter);
        // Assert
        ck_assert_int_eq(state, AuthPLen);
        ck_assert_int_eq(parser.UName[0], letter);
        ck_assert_int_eq(parser.UNamePosition, parser.ULen);
    }
END_TEST

START_TEST(Feed_ValidPLen_Succeeds)
    {
        // Arrange
        parser.State = AuthPLen;
        byte pLen = 5;
        // Act
        AuthParserState state = AuthParserFeed(&parser, pLen);
        // Assert
        ck_assert_int_eq(state, AuthPasswd);
        ck_assert_int_eq(parser.PLen, pLen);
    }
END_TEST

START_TEST(Feed_InvalidPLen_Fails)
    {
        // Arrange
        parser.State = AuthPLen;
        byte pLen = 0;
        // Act
        AuthParserState state = AuthParserFeed(&parser, pLen);
        // Assert
        ck_assert_int_eq(state, AuthInvalidState);
    }
END_TEST

START_TEST(Feed_PasswdIncomplete_Succeeds)
    {
        // Arrange
        parser.State = AuthPasswd;
        parser.PLen = 5;
        byte letter = 'L';
        // Act
        AuthParserState state = AuthParserFeed(&parser, letter);
        // Assert
        ck_assert_int_eq(state, AuthPasswd);
        ck_assert_ptr_ne(parser.Passwd, null);
        ck_assert_int_eq(parser.Passwd[0], letter);
        ck_assert_int_ne(parser.PasswdPosition, parser.PLen);
    }
END_TEST

START_TEST(Feed_PasswdComplete_Succeeds)
    {
        // Arrange
        parser.State = AuthPasswd;
        parser.PLen = 1;
        byte letter = 'L';
        // Act
        AuthParserState state = AuthParserFeed(&parser, letter);
        // Assert
        ck_assert_int_eq(state, AuthFinished);
        ck_assert_ptr_ne(parser.Passwd, null);
        ck_assert_int_eq(parser.Passwd[0], letter);
        ck_assert_int_eq(parser.PasswdPosition, parser.PLen);
    }
END_TEST

/**************************************************************************
 *                      AuthParserConsume Tests
 ***************************************************************************/

START_TEST(Consume_CompleteMessage_Succeeds)
    {
        // Arrange
        int messageLength = 9;
        byte uLen = 0x03;
        byte pLen = 0x03;
        byte message[] = {0x01, uLen, 'p', 'r', 'o', pLen, 't', 'o', 's'};
        // Act
        size_t consumedBytes = AuthParserConsume(&parser, message, messageLength);
        // Assert
        ck_assert_int_eq(consumedBytes, messageLength);
        ck_assert_int_eq(parser.State, AuthFinished);
        ck_assert_int_eq(parser.ULen, uLen);
        ck_assert_str_eq(parser.UName, "pro");
        ck_assert_int_eq(parser.PLen, pLen);
        ck_assert_str_eq(parser.Passwd, "tos");
    }
END_TEST

START_TEST(Consume_CompleteMessageWithExtraLength_Succeeds)
    {
        // Arrange
        int contentLength = 9;
        int messageLength = 12;
        byte uLen = 0x03;
        byte pLen = 0x03;
        byte message[] = {0x01, uLen, 'p', 'r', 'o', pLen, 't', 'o', 's', 0, 0, 0};
        // Act
        size_t consumedBytes = AuthParserConsume(&parser, message, messageLength);
        // Assert
        ck_assert_int_eq(consumedBytes, contentLength);
        ck_assert_int_eq(parser.State, AuthFinished);
        ck_assert_int_eq(parser.ULen, uLen);
        ck_assert_str_eq(parser.UName, "pro");
        ck_assert_int_eq(parser.PLen, pLen);
        ck_assert_str_eq(parser.Passwd, "tos");
    }
END_TEST

START_TEST(Consume_IncompleteMessage_Succeeds)
    {
        // Arrange
        int messageLength = 5;
        byte uLen = 0x03;
        byte message[] = {0x01, uLen, 'p', 'r', 'o'};
        // Act
        size_t consumedBytes = AuthParserConsume(&parser, message, messageLength);
        // Assert
        ck_assert_int_eq(consumedBytes, messageLength);
        ck_assert_int_eq(parser.State, AuthPLen);
        ck_assert_int_eq(parser.ULen, uLen);
        ck_assert_str_eq(parser.UName, "pro");
    }
END_TEST

START_TEST(Consume_NullParser_Fails)
    {
        // Arrange
        AuthParser *nullParser = null;
        int messageLength = 5;
        byte uLen = 0x03;
        byte message[] = {0x01, uLen, 'p', 'r', 'o'};
        // Act
        size_t consumedBytes = AuthParserConsume(nullParser, message, messageLength);
        // Assert
        ck_assert_int_eq(consumedBytes, 0);
    }
END_TEST


void SetUpAuthParser(void) {
    AuthParserReset(&parser);
}

void TeardownAuthParser(void) {
    AuthParserReset(&parser);
}

Suite *RegisterAuthParserTestSuit() {
    Suite *s = suite_create("AuthParser");

    TCase *tc = tcase_create("AuthParser");
    tcase_add_test(tc, Init_Succeeds);
    tcase_add_test(tc, Feed_NullParser_Fails);
    suite_add_tcase(s, tc);

    TCase *tc2 = tcase_create("AuthParser with TeardownHelloParser");
    tcase_add_checked_fixture(tc2, SetUpAuthParser, TeardownAuthParser);
    tcase_add_test(tc2, Feed_CorrectSocksVersion_Succeeds);
    tcase_add_test(tc2, Feed_IncorrectSocksVersion_Fails);
    tcase_add_test(tc2, Feed_ValidULen_Succeeds);
    tcase_add_test(tc2, Feed_InvalidULen_Fails);
    tcase_add_test(tc2, Feed_AuthUNameIncomplete_Succeeds);
    tcase_add_test(tc2, Feed_AuthUNameComplete_Succeeds);
    tcase_add_test(tc2, Feed_ValidPLen_Succeeds);
    tcase_add_test(tc2, Feed_InvalidPLen_Fails);
    tcase_add_test(tc2, Feed_PasswdIncomplete_Succeeds);
    tcase_add_test(tc2, Feed_PasswdComplete_Succeeds);
    tcase_add_test(tc2, Consume_CompleteMessage_Succeeds);
    tcase_add_test(tc2, Consume_CompleteMessageWithExtraLength_Succeeds);
    tcase_add_test(tc2, Consume_IncompleteMessage_Succeeds);
    tcase_add_test(tc2, Consume_NullParser_Fails);

    suite_add_tcase(s, tc2);

    return s;
}
