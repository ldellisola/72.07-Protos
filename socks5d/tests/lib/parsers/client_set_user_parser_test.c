//
// Created by tluci on 17/6/2022.
//

#include "client_set_user_parser_test.h"
#include "parsers/client_set_user_parser.h"
ClientSetUserParser parser;

/**************************************************************************
 *                      ClientSetUserParserReset Tests
 ***************************************************************************/
START_TEST(Reset_Succeeds)
    {

        // Arrange
        // Act
        ClientSetUserParserReset(&parser);
        // Assert
        for (int i = 0; i < MAXLONG; ++i) {
            ck_assert_int_eq(parser.Passwd[i], 0);
            ck_assert_int_eq(parser.UName[i], 0);
        }

        ck_assert_int_eq(parser.User[0], 'U');
        ck_assert_int_eq(parser.User[1], 'S');
        ck_assert_int_eq(parser.User[2], 'E');
        ck_assert_int_eq(parser.User[3], 'R');
        ck_assert_int_eq(parser.User[4], 0);
        ck_assert_int_eq(parser.Set[0], 'S');
        ck_assert_int_eq(parser.Set[1], 'E');
        ck_assert_int_eq(parser.Set[2], 'T');
        ck_assert_int_eq(parser.Set[3], 0);

        ck_assert_int_eq(parser.Word[0], 'S');
        ck_assert_int_eq(parser.Word[1], 'E');
        ck_assert_int_eq(parser.Word[2], 'T');
        ck_assert_int_eq(parser.Word[3], 0);
        ck_assert_int_eq(parser.State, UserSet);
        ck_assert_int_eq(parser.PrevState, UserSet);
        ck_assert_int_eq(parser.Index, 0);
    }
END_TEST

/**************************************************************************
 *                      SetUserParserFeed Tests
 ***************************************************************************/

START_TEST(Feed_NullParser_Fails)
    {
        // Arrange
        ClientSetUserParser *nullParser = null;
        // Act
        ClientSetUserParserState state = ClientSetUserParserFeed(nullParser, 0);
        // Assert
        ck_assert_int_eq(state, UserInvalidState);
    }
END_TEST

START_TEST(Feed_Complete_Succeeds)
    {

        // Arrange
        ClientSetUserParserReset(&parser);
        byte message[] = {'S', 'E','T','|','U','S','E','R','|','u','s','e','r','1','|','p','a','s','s','\r','\n'};
        ClientSetUserParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != UserInvalidState; i++ ){
            state = ClientSetUserParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, UserFinished);
        ck_assert_str_eq(parser.UName, "user1");
        ck_assert_str_eq(parser.Passwd, "pass");


    }
END_TEST

START_TEST(Feed_Complete_CR_Succeeds)
    {

        // Arrange
        ClientSetUserParserReset(&parser);
        byte message[] = {'S', 'E','T','|','U','S','E','R','|','u','s','\r','e','r','1','|','p','a','s','\r','s','\r','\n'};
        ClientSetUserParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != UserInvalidState; i++ ){
            state = ClientSetUserParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, UserFinished);
        ck_assert_str_eq(parser.UName, "us\rer1");
        ck_assert_str_eq(parser.Passwd, "pas\rs");


    }
END_TEST

START_TEST(Feed_Set_ExtraChar_Fails)
    {

        // Arrange
        ClientSetUserParserReset(&parser);
        byte message[] = {'S', 'E','T','O','|','U','S','E','R','|','u','s','e','r','1','|','p','a','s','s','\r','\n'};
        ClientSetUserParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != UserInvalidState; i++ ){
            state = ClientSetUserParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, UserInvalidState);
    }
END_TEST
START_TEST(Feed_Set_Short_Fails)
    {

        // Arrange
        ClientSetUserParserReset(&parser);
        byte message[] = {'S', 'E','|','U','S','E','R','|','u','s','e','r','1','|','p','a','s','s','\r','\n'};
        ClientSetUserParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != UserInvalidState; i++ ){
            state = ClientSetUserParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, UserInvalidState);
    }
END_TEST

START_TEST(Feed_User_ExtraChar_Fails)
    {

        // Arrange
        ClientSetUserParserReset(&parser);
        byte message[] = {'S', 'E','T','|','U','S','E','R','R','|','u','s','e','r','1','|','p','a','s','s','\r','\n'};
        ClientSetUserParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != UserInvalidState; i++ ){
            state = ClientSetUserParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, UserInvalidState);
    }
END_TEST
START_TEST(Feed_User_Short_Fails)
    {

        // Arrange
        ClientSetUserParserReset(&parser);
        byte message[] = {'S', 'E','T','|','U','S','E','|','u','s','e','r','1','|','p','a','s','s','\r','\n'};
        ClientSetUserParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != UserInvalidState; i++ ){
            state = ClientSetUserParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, UserInvalidState);
    }
END_TEST
START_TEST(Feed_UserName_CRLF_Fails)
    {

        // Arrange
        ClientSetUserParserReset(&parser);
        byte message[] = {'S', 'E','T','|','U','S','E','R','|','u','s','e','\r','\n','r','1','|','p','a','s','s','\r','\n'};
        ClientSetUserParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != UserInvalidState; i++ ){
            state = ClientSetUserParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, UserInvalidState);
    }
END_TEST

START_TEST(Feed_UserName_Empty_Fails)
    {

        // Arrange
        ClientSetUserParserReset(&parser);
        byte message[] = {'S', 'E','T','|','U','S','E','R','|','|','\r','\n'};
        ClientSetUserParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != UserInvalidState; i++ ){
            state = ClientSetUserParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, UserInvalidState);
    }
END_TEST
START_TEST(Feed_Password_Empty_Fails)
        {

                // Arrange
                ClientSetUserParserReset(&parser);
        byte message[] = {'S', 'E','T','|','U','S','E','R','|','a','|','\r','\n'};
        ClientSetUserParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != UserInvalidState; i++ ){
            state = ClientSetUserParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, UserInvalidState);
        }
END_TEST

START_TEST(Feed_Password_CR_Succeeds)
    {

        // Arrange
        ClientSetUserParserReset(&parser);
        byte message[] = {'S', 'E','T','|','U','S','E','R','|','a','|','\r','\r','\n'};
        ClientSetUserParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != UserInvalidState; i++ ){
            state = ClientSetUserParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, UserFinished);
    }
END_TEST

Suite *RegisterClientSetUserParserTestSuit() {
    Suite *s = suite_create("SetUserClientParser");

    TCase *tc = tcase_create("SetUserClientParser");
    tcase_add_test(tc, Reset_Succeeds);
    tcase_add_test(tc, Feed_NullParser_Fails);
    tcase_add_test(tc, Feed_Complete_Succeeds);
    tcase_add_test(tc, Feed_Complete_CR_Succeeds);
    tcase_add_test(tc, Feed_Set_ExtraChar_Fails);
    tcase_add_test(tc, Feed_Set_Short_Fails);
    tcase_add_test(tc, Feed_User_ExtraChar_Fails);
    tcase_add_test(tc, Feed_User_Short_Fails);
    tcase_add_test(tc, Feed_UserName_CRLF_Fails);
    tcase_add_test(tc, Feed_UserName_Empty_Fails);
    tcase_add_test(tc, Feed_Password_Empty_Fails);
    tcase_add_test(tc, Feed_Password_CR_Succeeds);
    suite_add_tcase(s, tc);

    return s;
}
