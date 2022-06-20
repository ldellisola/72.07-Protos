//
// Created by tluci on 17/6/2022.
//

#include "client_timeout_parser_test.h"
#include "parsers/lulu/client_timeout_parser.h"
static ClientTimeoutParser parser;

/**************************************************************************
 *                      ClientHelloParserReset Tests
 ***************************************************************************/
START_TEST(Reset_Succeeds)
        {

                // Arrange
                // Act
                ClientTimeoutParserReset(&parser);
        // Assert
        ck_assert_int_eq(parser.Timeout[0], 'T');
        ck_assert_int_eq(parser.Timeout[1], 'I');
        ck_assert_int_eq(parser.Timeout[2], 'M');
        ck_assert_int_eq(parser.Timeout[3], 'E');
        ck_assert_int_eq(parser.Timeout[4], 'O');
        ck_assert_int_eq(parser.Timeout[5], 'U');
        ck_assert_int_eq(parser.Timeout[6], 'T');
        ck_assert_int_eq(parser.Set[0], 'S');
        ck_assert_int_eq(parser.Set[1], 'E');
        ck_assert_int_eq(parser.Set[2], 'T');
        ck_assert_int_eq(parser.State, TimeoutSet);
        ck_assert_int_eq(parser.Index, 0);
        ck_assert_int_eq(parser.Value, 0);
        }
END_TEST
/**************************************************************************
 *                      AuthParserFeed Tests
 ***************************************************************************/

START_TEST(Feed_NullParser_Fails)
{
    // Arrange
    ClientTimeoutParser *nullParser = null;
    // Act
    ClientTimeoutParserState state = ClientTimeoutParserFeed(nullParser, 0);
    // Assert
    ck_assert_int_eq(state, TimeoutInvalidState);
}
END_TEST
//

START_TEST(Feed_Complete_Succeeds)
{

    // Arrange
    ClientTimeoutParserReset(&parser);
    byte message[] = {'S','E', 'T', '|','T','I','M','E','O','U','T','|','5','3','4','\r', '\n'};
    ClientTimeoutParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != TimeoutInvalidState; i++ ){
        state = ClientTimeoutParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, TimeoutFinished);
    ck_assert_int_eq(parser.Value, 534);
}
END_TEST

//
//START_TEST(Feed_Value_Fails)
//{
//
//    // Arrange
//    ClientTimeoutParserReset(&parser);
//    byte message[] = {'S','E', 'T', '|','T','I','M','E','O','U','T','|','5','a','3','4','\r', '\n'};
//    ClientTimeoutParserState state;
//    // Act
//    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != TimeoutInvalidState; i++ ){
//        state = ClientTimeoutParserFeed(&parser, message[i]);
//    }
//
//    // Assert
//    ck_assert_int_eq(state, TimeoutInvalidState);
//}
//END_TEST
//START_TEST(Feed_CRLF_Fails)
//{
//
//    // Arrange
//    ClientTimeoutParserReset(&parser);
//    byte message[] = {'S','E', 'T', '|','T','I','M','E','O','U','T','|','5','3','4','\r',0 };
//    ClientTimeoutParserState state;
//    // Act
//    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != TimeoutInvalidState; i++ ){
//        state = ClientTimeoutParserFeed(&parser, message[i]);
//    }
//
//    // Assert
//    ck_assert_int_eq(state, TimeoutInvalidState);
//}
//END_TEST
//
//START_TEST(Feed_Complete_ExtraChar_Succeeds)
//{
//
//    // Arrange
//    ClientTimeoutParserReset(&parser);
//    byte message[] = {'S','E', 'T', '|','T','I','M','E','O','U','T','|','5','3','4','\r','\n', '3' };
//    ClientTimeoutParserState state;
//    // Act
//    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != TimeoutInvalidState; i++ ){
//        state = ClientTimeoutParserFeed(&parser, message[i]);
//    }
//
//    // Assert
//    ck_assert_int_eq(state, TimeoutFinished);
//}
//END_TEST
//
//START_TEST(Feed_Set_Succeeds)
//{
//
//    // Arrange
//    ClientTimeoutParserReset(&parser);
//    byte message[] = {'S','E', 'T', '|' };
//    ClientTimeoutParserState state;
//    // Act
//    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != TimeoutInvalidState; i++ ){
//        state = ClientTimeoutParserFeed(&parser, message[i]);
//    }
//
//    // Assert
//    ck_assert_int_eq(state, Timeout);
//}
//END_TEST
//START_TEST(Feed_Timeout_Succeeds)
//{
//
//    // Arrange
//    ClientTimeoutParserReset(&parser);
//    byte message[] = {'S','E', 'T', '|','T','I','M','E','O','U','T','|' };
//    ClientTimeoutParserState state;
//    // Act
//    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != TimeoutInvalidState; i++ ){
//        state = ClientTimeoutParserFeed(&parser, message[i]);
//    }
//
//    // Assert
//    ck_assert_int_eq(state, TimeoutValue);
//}
//END_TEST
//START_TEST(Feed_Value_Succeeds)
//{
//
//    // Arrange
//    ClientTimeoutParserReset(&parser);
//    byte message[] = {'S','E', 'T', '|','T','I','M','E','O','U','T','|','5','3','4','\r' };
//    ClientTimeoutParserState state;
//    // Act
//    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != TimeoutInvalidState; i++ ){
//        state = ClientTimeoutParserFeed(&parser, message[i]);
//    }
//
//    // Assert
//    ck_assert_int_eq(state, TimeoutCRLF);
//}
//END_TEST


Suite *RegisterClientTimeoutParserTestSuit() {
    Suite *s = suite_create("TimeoutClientParser");

    TCase *tc = tcase_create("TimeoutClientParser");
    tcase_add_test(tc, Reset_Succeeds);
    tcase_add_test(tc, Feed_NullParser_Fails);
//    tcase_add_test(tc, Feed_Complete_ExtraChar_Succeeds );
//    tcase_add_test(tc, Feed_CRLF_Fails);
    tcase_add_test(tc, Feed_Complete_Succeeds);
//    tcase_add_test(tc, Feed_Set_Succeeds);
//    tcase_add_test(tc, Feed_Timeout_Succeeds);
//    tcase_add_test(tc, Feed_Value_Fails);
//    tcase_add_test(tc, Feed_Value_Succeeds);

    suite_add_tcase(s, tc);

    return s;
}

