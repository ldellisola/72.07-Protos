//
// Created by tluci on 16/6/2022.
//

#include "client_goodbye_parser_test.h"
#include "parsers/client_goodbye_parser.h"
ClientGoodbyeParser parser;

/**************************************************************************
 *                      ClientGoodbyeParserReset Tests
 ***************************************************************************/
START_TEST(Reset_Succeeds)
    {

        // Arrange
        // Act
        ClientGoodbyeParserReset(&parser);
        // Assert

        ck_assert_int_eq(parser.Goodbye[0], 'G');
        ck_assert_int_eq(parser.Goodbye[1], 'O');
        ck_assert_int_eq(parser.Goodbye[2], 'O');
        ck_assert_int_eq(parser.Goodbye[3], 'D');
        ck_assert_int_eq(parser.Goodbye[4], 'B');
        ck_assert_int_eq(parser.Goodbye[5], 'Y');
        ck_assert_int_eq(parser.Goodbye[6], 'E');
        ck_assert_int_eq(parser.State, Goodbye);
        ck_assert_int_eq(parser.index, 0);
    }
END_TEST
/**************************************************************************
 *                      GoodbyeParserFeed Tests
 ***************************************************************************/

START_TEST(Feed_NullParser_Fails)
    {
        // Arrange
        ClientGoodbyeParser *nullParser = null;
        // Act
        ClientGoodbyeParserState state = ClientGoodbyeParserFeed(nullParser, 0);
        // Assert
        ck_assert_int_eq(state, GoodbyeInvalidState);
    }
END_TEST

START_TEST(Feed_GOODBYE_COMPLETE_Succeeds)
    {

        // Arrange
        ClientGoodbyeParserReset(&parser);
        byte message[] = {'G','O', 'O', 'D', 'B', 'Y', 'E', '\r', '\n'};
        ClientGoodbyeParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != GoodbyeInvalidState; i++ ){
            state = ClientGoodbyeParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, GoodbyeFinished);
    }
END_TEST

START_TEST(Feed_GOODBYE_Short_Fails)
    {

        // Arrange
        ClientGoodbyeParserReset(&parser);
        byte message[] = {'G','O', 'O', 'D',  '\r', '\n'};
        ClientGoodbyeParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != GoodbyeInvalidState; i++ ){
            state = ClientGoodbyeParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, GoodbyeInvalidState);
    }
END_TEST
START_TEST(Feed_GOODBYE_Long_Fails)
    {

        // Arrange
        ClientGoodbyeParserReset(&parser);
        byte message[] = {'G','O', 'O', 'D','B', 'Y', 'E',  '\r','E', '\n'};
        ClientGoodbyeParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != GoodbyeInvalidState; i++ ){
            state = ClientGoodbyeParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, GoodbyeInvalidState);
    }
END_TEST

START_TEST(Feed_GOODBYE_noCRLF_Fails)
    {

        // Arrange
        ClientGoodbyeParserReset(&parser);
        byte message[] = {'G','O', 'O', 'D','B', 'Y', 'E'};
        ClientGoodbyeParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != GoodbyeInvalidState; i++ ){
            state = ClientGoodbyeParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, Goodbye);
    }
END_TEST





/**************************************************************************
 *                      GoodbyeParserConsume Tests
 ***************************************************************************/

START_TEST(Consume_CompleteMessage_Succeeds)
    {
        // Arrange
        ClientGoodbyeParserReset(&parser);
        int messageLength = 9;
        byte message[] = {'G','O', 'O', 'D','B', 'Y', 'E', '\r', '\n'};
        // Act
        int consumedBytes = ClientGoodbyeParserConsume(&parser, message, messageLength);
        // Assert
        ck_assert_int_eq(consumedBytes, messageLength);
        ck_assert_int_eq(parser.State, GoodbyeFinished);
    }
END_TEST

START_TEST(Consume_CompleteMessageWithExtraLength_Succeeds)
    {
        // Arrange
        ClientGoodbyeParserReset(&parser);
        int contentLength = 9;
        int messageLength = 12;
        byte message[] = {'G','O', 'O', 'D','B', 'Y', 'E', '\r', '\n', 0, 0};
        // Act
        int consumedBytes = ClientGoodbyeParserConsume(&parser, message, messageLength);
        // Assert
        ck_assert_int_eq(consumedBytes, contentLength);
        ck_assert_int_eq(parser.State, GoodbyeFinished);
    }
END_TEST

START_TEST(Consume_IncompleteGoodbye_Succeeds)
    {
        // Arrange
        ClientGoodbyeParserReset(&parser);
        int messageLength = 5;
        byte message[] = {'G','O', 'O', 'D','B', 'Y', 'E'};
        // Act
        int consumedBytes = ClientGoodbyeParserConsume(&parser, message, messageLength);
        // Assert
        ck_assert_int_eq(consumedBytes, messageLength);
        ck_assert_int_eq(parser.State, Goodbye);
    }
END_TEST

START_TEST(Consume_NullParser_Fails)
    {
        // Arrange
        ClientGoodbyeParser *nullParser = null;
        int messageLength = 9;
        byte message[] = {'G','O', 'O', 'D','B', 'Y', 'E', '\r', '\n'};
        // Act
        int consumedBytes = ClientGoodbyeParserConsume(nullParser, message, messageLength);
        // Assert
        ck_assert_int_eq(consumedBytes, 0);
    }
END_TEST

Suite *RegisterClientGoodbyeParserTestSuit() {
    Suite *s = suite_create("GoodbyeClientParser");

    TCase *tc = tcase_create("GoodbyeClientParser");
    tcase_add_test(tc, Reset_Succeeds);
    tcase_add_test(tc, Feed_NullParser_Fails);
    tcase_add_test(tc, Feed_GOODBYE_COMPLETE_Succeeds);
    tcase_add_test(tc, Feed_GOODBYE_Short_Fails);
    tcase_add_test(tc, Feed_GOODBYE_Long_Fails);
    tcase_add_test(tc, Feed_GOODBYE_noCRLF_Fails);
    tcase_add_test(tc, Consume_CompleteMessageWithExtraLength_Succeeds);
    tcase_add_test(tc, Consume_CompleteMessage_Succeeds);
    tcase_add_test(tc, Consume_IncompleteGoodbye_Succeeds);
    tcase_add_test(tc, Consume_NullParser_Fails);
    suite_add_tcase(s, tc);

    return s;
}
