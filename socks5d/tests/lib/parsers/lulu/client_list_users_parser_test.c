//
// Created by tluci on 17/6/2022.
//

#include "client_list_users_parser_test.h"
#include "parsers/lulu/client_list_users_parser.h"

ClientListUsersParser parser;

/**************************************************************************
 *                      ClientListUsersParserReset Tests
 ***************************************************************************/
START_TEST(Reset_Succeeds)
    {
        // Arrange
        // Act
        ClientListUsersParserReset(&parser);

        // Assert
        ck_assert_int_eq(parser.List[0], 'L');
        ck_assert_int_eq(parser.List[1], 'I');
        ck_assert_int_eq(parser.List[2], 'S');
        ck_assert_int_eq(parser.List[3], 'T');
        ck_assert_int_eq(parser.List[4], 0);

        ck_assert_int_eq(parser.Users[0], 'U');
        ck_assert_int_eq(parser.Users[1], 'S');
        ck_assert_int_eq(parser.Users[2], 'E');
        ck_assert_int_eq(parser.Users[3], 'R');
        ck_assert_int_eq(parser.Users[4], 'S');
        ck_assert_int_eq(parser.Users[5], 0);

        ck_assert_int_eq(parser.State, List);
        ck_assert_int_eq(parser.Index, 0);
    }
END_TEST
///**************************************************************************
/// *                      ListUsersParserFeed Tests
/// ***************************************************************************/

START_TEST(Feed_NullParser_Fails)
    {
        // Arrange
        ClientListUsersParser *nullParser = null;
        // Act
        ClientListUsersParserState state = ClientListUsersParserFeed(nullParser, 0);
        // Assert
        ck_assert_int_eq(state, ListInvalidState);
    }
END_TEST

START_TEST(Feed_Complete_Succeeds)
    {

        // Arrange
        ClientListUsersParserReset(&parser);
        byte message[] = {'L','I','S','T','|','U','S','E','R','S','\r', '\n'};
        ClientListUsersParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != ListInvalidState; i++ ){
            state = ClientListUsersParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, ListFinished);
    }
END_TEST

START_TEST(Feed_List_ExtraChar_Fails)
    {

        // Arrange
        ClientListUsersParserReset(&parser);
        byte message[] = {'L','I','S','T','f','|','U','S','E','R','S','\r', '\n'};
        ClientListUsersParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != ListInvalidState; i++ ){
            state = ClientListUsersParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, ListInvalidState);
    }
END_TEST

START_TEST(Feed_Users_ExtraChar_Fails)
    {

        // Arrange
        ClientListUsersParserReset(&parser);
        byte message[] = {'L','I','S','T','|','U','S','E','R','S','\r','\r', '\n'};
        ClientListUsersParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != ListInvalidState; i++ ){
            state = ClientListUsersParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, ListInvalidState);
    }
END_TEST


START_TEST(Feed_CRLF_Fails)
    {

        // Arrange
        ClientListUsersParserReset(&parser);
        byte message[] = {'L','I','S','T','|','U','S','E','R','S','\r', 'o','\n'};
        ClientListUsersParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != ListInvalidState; i++ ){
            state = ClientListUsersParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, ListInvalidState);
    }
END_TEST

START_TEST(Feed_Complete_ExtraChar_Succeeds)
    {

        // Arrange
        ClientListUsersParserReset(&parser);
        byte message[] = {'L','I','S','T','|','U','S','E','R','S','\r', '\n', 'o'};
        ClientListUsersParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != ListInvalidState; i++ ){
            state = ClientListUsersParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, ListFinished);
    }
END_TEST

START_TEST(Feed_List_Succeeds)
    {

        // Arrange
        ClientListUsersParserReset(&parser);
        byte message[] = {'L','I','S','T','|'};
        ClientListUsersParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != ListInvalidState; i++ ){
            state = ClientListUsersParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, ListUsers);
    }
END_TEST

START_TEST(Feed_List_Fails)
    {

        // Arrange
        ClientListUsersParserReset(&parser);
        byte message[] = {'G','|' };
        ClientListUsersParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != ListInvalidState; i++ ){
            state = ClientListUsersParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, ListInvalidState);
    }
END_TEST

START_TEST(Feed_Users_Succeeds)
    {

        // Arrange
        ClientListUsersParserReset(&parser);
        byte message[] = {'L','I','S','T','|','U','S','E','R','S','\r',};
        ClientListUsersParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != ListInvalidState; i++ ){
            state = ClientListUsersParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, ListCRLF);
    }
END_TEST

START_TEST(Feed_Users_Fails)
    {

        // Arrange
        ClientListUsersParserReset(&parser);
        byte message[] = {'L','I','S','T','|','U','S','E','R','\r'};
        ClientListUsersParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != ListInvalidState; i++ ){
            state = ClientListUsersParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, ListInvalidState);
    }
END_TEST



Suite *RegisterClientListUsersParserTestSuit() {
    Suite *s = suite_create("ClientListUsersParser");

    TCase *tc = tcase_create("ClientListUsersParser");
    tcase_add_test(tc, Reset_Succeeds);
    tcase_add_test(tc, Feed_NullParser_Fails);
    tcase_add_test(tc, Feed_Complete_ExtraChar_Succeeds );
    tcase_add_test(tc, Feed_CRLF_Fails);
    tcase_add_test(tc, Feed_Complete_Succeeds);
    tcase_add_test(tc, Feed_Users_Fails);
    tcase_add_test(tc, Feed_Users_Succeeds);
    tcase_add_test(tc, Feed_List_Fails);
    tcase_add_test(tc, Feed_List_Succeeds);
    tcase_add_test(tc, Feed_Users_ExtraChar_Fails);
    tcase_add_test(tc, Feed_List_ExtraChar_Fails);

    suite_add_tcase(s, tc);

    return s;
}
