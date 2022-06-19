//
// Created by tluci on 17/6/2022.
//

#include "client_del_user_parser_test.h"
#include "parsers/lulu/client_del_user_parser.h"
ClientDelUserParser parser;

/**************************************************************************
 *                      ClientDelUserParserReset Tests
 ***************************************************************************/
START_TEST(Reset_Succeeds)
    {

        // Arrange
        // Act
        ClientDelUserParserReset(&parser);
        // Assert
        for (int i = 0; i < MAXLONG; ++i) {
            ck_assert_int_eq(parser.UName[i], 0);
        }
        ck_assert_int_eq(parser.Word[0], 'D');
        ck_assert_int_eq(parser.Word[1], 'E');
        ck_assert_int_eq(parser.Word[2], 'L');
        ck_assert_int_eq(parser.Word[3], 0);
        ck_assert_int_eq(parser.Del[0], 'D');
        ck_assert_int_eq(parser.Del[1], 'E');
        ck_assert_int_eq(parser.Del[2], 'L');
        ck_assert_int_eq(parser.Del[3], 0);
        ck_assert_int_eq(parser.User[0], 'U');
        ck_assert_int_eq(parser.User[1], 'S');
        ck_assert_int_eq(parser.User[2], 'E');
        ck_assert_int_eq(parser.User[3], 'R');
        ck_assert_int_eq(parser.User[4], 0);
        ck_assert_int_eq(parser.State, Del);
        ck_assert_int_eq(parser.Index, 0);
    }
END_TEST
/**************************************************************************
 *                      DelUserParserFeed Tests
 ***************************************************************************/

START_TEST(Feed_NullParser_Fails)
    {
        // Arrange
        ClientDelUserParser *nullParser = null;
        // Act
        ClientDelUserParserState state = ClientDelUserParserFeed(nullParser, 0);
        // Assert
        ck_assert_int_eq(state, DelInvalidState);
    }
END_TEST

START_TEST(Feed_Complete_Succeeds)
    {

        // Arrange
        ClientDelUserParserReset(&parser);
        byte message[] = {'D', 'E', 'L','|', 'U','S','E','R', '|', 'u', 's', 'e','r','\r', '1', '\r', '\n'};
        ClientDelUserParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != DelInvalidState; i++ ){
            state = ClientDelUserParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, DelFinished);
        ck_assert_str_eq(parser.UName, "user\r1");


    }
END_TEST
START_TEST(Feed_Username_CR_Succeeds)
    {

        // Arrange
        ClientDelUserParserReset(&parser);
        byte message[] = {'D', 'E', 'L','|', 'U','S','E','R', '|', '\r',  '\r', '\n'};
        ClientDelUserParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != DelInvalidState; i++ ){
            state = ClientDelUserParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, DelFinished);
        ck_assert_str_eq(parser.UName, "\r");


    }
END_TEST
START_TEST(Feed_Username_Empty_Fails)
{

    // Arrange
    ClientDelUserParserReset(&parser);
    byte message[] = {'D', 'E', 'L','|', 'U','S','E','R', '|', '\r', '\n'};
    ClientDelUserParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != DelInvalidState; i++ ){
        state = ClientDelUserParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, DelInvalidState);


}
END_TEST


START_TEST(Feed_CRLF_Fails)
    {

        // Arrange
        ClientDelUserParserReset(&parser);
        byte message[] = {'D', 'E', 'L','|','\r','\n', 'U','S','E','R', '|', 'u', 's', 'e','r','\r', '1', '\r', '\n'};
        ClientDelUserParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != DelInvalidState; i++ ){
            state = ClientDelUserParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, DelInvalidState);
    }
END_TEST



START_TEST(Feed_Del_Succeeds)
    {

        // Arrange
        ClientDelUserParserReset(&parser);

        byte message[] = {'D', 'E', 'L','|'};
        ClientDelUserParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != DelInvalidState; i++ ){
            state = ClientDelUserParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, DelUser);
    }
END_TEST





Suite *RegisterClientDelUserParserTestSuit() {
    Suite *s = suite_create("ClientDelUserParser");

    TCase *tc = tcase_create("ClientDelUserParser");
    tcase_add_test(tc, Reset_Succeeds);
    tcase_add_test(tc, Feed_NullParser_Fails);
    tcase_add_test(tc, Feed_Complete_Succeeds);
    tcase_add_test(tc, Feed_Username_CR_Succeeds);
    tcase_add_test(tc, Feed_Username_Empty_Fails);
    tcase_add_test(tc, Feed_CRLF_Fails);
    tcase_add_test(tc, Feed_Del_Succeeds);
    suite_add_tcase(s, tc);

    return s;
}

