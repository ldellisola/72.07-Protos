//
// Created by tluci on 17/6/2022.
//

#include "client_get_timeout_parser_test.h"
#include "parsers/client_get_timeout_parser.h"
ClientGetTimeoutParser parser;

/**************************************************************************
 *                      ClientGetTimeoutParserReset Tests
 ***************************************************************************/
START_TEST(Reset_Succeeds)
{
        // Arrange
        // Act
        ClientGetTimeoutParserReset(&parser);
        // Assert
        ck_assert_int_eq(parser.Timeout[0], 'T');
        ck_assert_int_eq(parser.Timeout[1], 'I');
        ck_assert_int_eq(parser.Timeout[2], 'M');
        ck_assert_int_eq(parser.Timeout[3], 'E');
        ck_assert_int_eq(parser.Timeout[4], 'O');
        ck_assert_int_eq(parser.Timeout[5], 'U');
        ck_assert_int_eq(parser.Timeout[6], 'T');
        ck_assert_int_eq(parser.Timeout[7], 0);
        ck_assert_int_eq(parser.Get[0], 'G');
        ck_assert_int_eq(parser.Get[1], 'E');
        ck_assert_int_eq(parser.Get[2], 'T');
        ck_assert_int_eq(parser.Get[3], 0);
        ck_assert_int_eq(parser.State, TimeoutGet);
        ck_assert_int_eq(parser.Index, 0);
        }
END_TEST
///**************************************************************************
/// *                      GetTimeoutParserFeed Tests
/// ***************************************************************************/

START_TEST(Feed_NullParser_Fails)
{
    // Arrange
    ClientGetTimeoutParser *nullParser = null;
    // Act
    ClientGetTimeoutParserState state = ClientGetTimeoutParserFeed(nullParser, 0);
    // Assert
    ck_assert_int_eq(state, TimeoutInvalidState);
}
END_TEST

START_TEST(Feed_Complete_Succeeds)
{

    // Arrange
    ClientGetTimeoutParserReset(&parser);
    byte message[] = {'G','E', 'T', '|','T','I','M','E','O','U','T','\r', '\n'};
    ClientGetTimeoutParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != TimeoutInvalidState; i++ ){
        state = ClientGetTimeoutParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, TimeoutFinished);
}
END_TEST

START_TEST(Feed_ExtraChar_Get_Fails)
    {

        // Arrange
        ClientGetTimeoutParserReset(&parser);
        byte message[] = {'G','E', 'T', 'O','|','T','I','M','E','O','U','T','\r', '\n'};
        ClientGetTimeoutParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != TimeoutInvalidState; i++ ){
            state = ClientGetTimeoutParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, TimeoutInvalidState);
    }
END_TEST

START_TEST(Feed_ExtraChar_Timeout_Fails)
    {

        // Arrange
        ClientGetTimeoutParserReset(&parser);
        byte message[] = {'G','E', 'T', '|','T','I','M','E','O','U','T','O','\r', '\n'};
        ClientGetTimeoutParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != TimeoutInvalidState; i++ ){
            state = ClientGetTimeoutParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, TimeoutInvalidState);
    }
END_TEST


START_TEST(Feed_CRLF_Fails)
{

    // Arrange
    ClientGetTimeoutParserReset(&parser);
    byte message[] = {'G','E', 'T', '|','T','I','M','E','O','U','T','\r',0 };
    ClientGetTimeoutParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != TimeoutInvalidState; i++ ){
        state = ClientGetTimeoutParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, TimeoutInvalidState);
}
END_TEST

START_TEST(Feed_Complete_ExtraChar_Succeeds)
{

    // Arrange
    ClientGetTimeoutParserReset(&parser);
    byte message[] = {'G','E', 'T', '|','T','I','M','E','O','U','T','\r','\n', '3' };
    ClientGetTimeoutParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != TimeoutInvalidState; i++ ){
        state = ClientGetTimeoutParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, TimeoutFinished);
}
END_TEST

START_TEST(Feed_Get_Succeeds)
{

    // Arrange
    ClientGetTimeoutParserReset(&parser);
    byte message[] = {'G','E', 'T', '|' };
    ClientGetTimeoutParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != TimeoutInvalidState; i++ ){
        state = ClientGetTimeoutParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, Timeout);
}
END_TEST

START_TEST(Feed_Get_Fails)
    {

        // Arrange
        ClientGetTimeoutParserReset(&parser);
        byte message[] = {'G','|' };
        ClientGetTimeoutParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != TimeoutInvalidState; i++ ){
            state = ClientGetTimeoutParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, TimeoutInvalidState);
    }
END_TEST

START_TEST(Feed_Timeout_Succeeds)
{

    // Arrange
    ClientGetTimeoutParserReset(&parser);
    byte message[] = {'G','E', 'T', '|','T','I','M','E','O','U','T','\r' };
    ClientGetTimeoutParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != TimeoutInvalidState; i++ ){
        state = ClientGetTimeoutParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, TimeoutCRLF);
}
END_TEST

START_TEST(Feed_Timeout_Fails)
    {

        // Arrange
        ClientGetTimeoutParserReset(&parser);
        byte message[] = {'G','E', 'T', '|','T','I','R','E','O','T','\r' };
        ClientGetTimeoutParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != TimeoutInvalidState; i++ ){
            state = ClientGetTimeoutParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, TimeoutInvalidState);
    }
END_TEST



Suite *RegisterClientGetTimeoutParserTestSuit() {
    Suite *s = suite_create("GetTimeoutClientParser");

    TCase *tc = tcase_create("GetTimeoutClientParser");
    tcase_add_test(tc, Reset_Succeeds);
    tcase_add_test(tc, Feed_NullParser_Fails);
    tcase_add_test(tc, Feed_Complete_ExtraChar_Succeeds );
    tcase_add_test(tc, Feed_CRLF_Fails);
    tcase_add_test(tc, Feed_Complete_Succeeds);
    tcase_add_test(tc, Feed_ExtraChar_Get_Fails);
    tcase_add_test(tc, Feed_ExtraChar_Timeout_Fails);
    tcase_add_test(tc, Feed_Get_Succeeds);
    tcase_add_test(tc, Feed_Timeout_Succeeds);
    tcase_add_test(tc, Feed_Timeout_Fails);
    tcase_add_test(tc, Feed_Get_Fails);

    suite_add_tcase(s, tc);

    return s;
}

