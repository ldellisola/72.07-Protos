//
// Created by tluci on 17/6/2022.
//

#include "client_set_buffersize_parser_test.h"
#include "parsers/client_set_buffersize_parser.h"
ClientSetBufferSizeParser parser;

/**************************************************************************
 *                      ClientSetBufferSizeParserReset Tests
 ***************************************************************************/
START_TEST(Reset_Succeeds)
    {

        // Arrange
        // Act
        ClientSetBufferSizeParserReset(&parser);
        // Assert
        ck_assert_int_eq(parser.BufferSize[0], 'B');
        ck_assert_int_eq(parser.BufferSize[1], 'U');
        ck_assert_int_eq(parser.BufferSize[2], 'F');
        ck_assert_int_eq(parser.BufferSize[3], 'F');
        ck_assert_int_eq(parser.BufferSize[4], 'E');
        ck_assert_int_eq(parser.BufferSize[5], 'R');
        ck_assert_int_eq(parser.BufferSize[6], 'S');
        ck_assert_int_eq(parser.BufferSize[7], 'I');
        ck_assert_int_eq(parser.BufferSize[8], 'Z');
        ck_assert_int_eq(parser.BufferSize[9], 'E');
        ck_assert_int_eq(parser.BufferSize[10], 0);
        ck_assert_int_eq(parser.Set[0], 'S');
        ck_assert_int_eq(parser.Set[1], 'E');
        ck_assert_int_eq(parser.Set[2], 'T');
        ck_assert_int_eq(parser.Set[3], 0);
        ck_assert_int_eq(parser.State, BufferSizeSet);
        ck_assert_int_eq(parser.Index, 0);
        ck_assert_int_eq(parser.Value, 0);
    }
END_TEST
/**************************************************************************
 *                      SetBufferSizeParserFeed Tests
 ***************************************************************************/

START_TEST(Feed_NullParser_Fails)
    {
        // Arrange
        ClientSetBufferSizeParser *nullParser = null;
        // Act
        ClientSetBufferSizeParserState state = ClientSetBufferSizeParserFeed(nullParser, 0);
        // Assert
        ck_assert_int_eq(state, BufferSizeInvalidState);
    }
END_TEST


START_TEST(Feed_Complete_Succeeds)
    {

        // Arrange
        ClientSetBufferSizeParserReset(&parser);
        byte message[] = {'S','E', 'T', '|','B','U','F','F','E','R','S','I','Z','E','|','5', '6', '4','\r', '\n'};
        ClientSetBufferSizeParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != BufferSizeInvalidState; i++ ){
            state = ClientSetBufferSizeParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, BufferSizeFinished);
        ck_assert_int_eq(parser.Value, 564);
    }
END_TEST
//
//
START_TEST(Feed_Value_Fails)
    {

        // Arrange
        ClientSetBufferSizeParserReset(&parser);
        byte message[] = {'S','E', 'T', '|','B','U','F','F','E','R','S','I','Z','E','|','5', '-', '4','\r', '\n'};
        ClientSetBufferSizeParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != BufferSizeInvalidState; i++ ){
            state = ClientSetBufferSizeParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, BufferSizeInvalidState);
    }
END_TEST
START_TEST(Feed_CRLF_Fails)
    {

        // Arrange
        ClientSetBufferSizeParserReset(&parser);
        byte message[] = {'S','E', 'T', '|','B','U','F','F','E','R','S','I','Z','E','|','5', '6', '4','\r', 0};
        ClientSetBufferSizeParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != BufferSizeInvalidState; i++ ){
            state = ClientSetBufferSizeParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, BufferSizeInvalidState);
        ck_assert_int_eq(parser.Value, 564);
    }
END_TEST

START_TEST(Feed_Complete_ExtraChar_Succeeds)
    {

        // Arrange
        ClientSetBufferSizeParserReset(&parser);
        byte message[] = {'S','E', 'T', '|','B','U','F','F','E','R','S','I','Z','E','|','5', '6', '4','\r', '\n', 'f', '3'};
        ClientSetBufferSizeParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != BufferSizeInvalidState; i++ ){
            state = ClientSetBufferSizeParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, BufferSizeFinished);
    }
END_TEST

START_TEST(Feed_Set_Succeeds)
    {

        // Arrange
        ClientSetBufferSizeParserReset(&parser);
        byte message[] = {'S','E', 'T', '|' };
        ClientSetBufferSizeParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != BufferSizeInvalidState; i++ ){
            state = ClientSetBufferSizeParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, BufferSize);
    }
END_TEST
START_TEST(Feed_BufferSize_Succeeds)
    {

        // Arrange
        ClientSetBufferSizeParserReset(&parser);
        byte message[] = {'S','E', 'T', '|','B','U','F','F','E','R','S','I','Z','E','|'};
        ClientSetBufferSizeParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != BufferSizeInvalidState; i++ ){
            state = ClientSetBufferSizeParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, BufferSizeValue);
    }
END_TEST
START_TEST(Feed_Value_Succeeds)
    {

        // Arrange
        ClientSetBufferSizeParserReset(&parser);
        byte message[] = {'S','E', 'T', '|','B','U','F','F','E','R','S','I','Z','E','|','5', '3', '4','\r', };
        ClientSetBufferSizeParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != BufferSizeInvalidState; i++ ){
            state = ClientSetBufferSizeParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, BufferSizeCRLF);
        ck_assert_int_eq(parser.Value, 534);
    }
END_TEST

START_TEST(Feed_Value_Big_Fails)
    {

        // Arrange
        ClientSetBufferSizeParserReset(&parser);
        byte message[] = {'S','E', 'T', '|','B','U','F','F','E','R','S','I','Z','E','|','5', '6', '4','5', '6', '4','5', '6', '4','5', '6', '4','\r', '\n'};
        ClientSetBufferSizeParserState state;
        // Act
        for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != BufferSizeInvalidState; i++ ){
            state = ClientSetBufferSizeParserFeed(&parser, message[i]);
        }

        // Assert
        ck_assert_int_eq(state, BufferSizeInvalidState);
    }
END_TEST


Suite *RegisterClientSetBufferSizeParserTestSuit() {
    Suite *s = suite_create("SetBufferSizeClientParser");

    TCase *tc = tcase_create("SetBufferSizeClientParser");
    tcase_add_test(tc, Reset_Succeeds);
    tcase_add_test(tc, Feed_NullParser_Fails);
    tcase_add_test(tc, Feed_Complete_ExtraChar_Succeeds );
    tcase_add_test(tc, Feed_CRLF_Fails);
    tcase_add_test(tc, Feed_Complete_Succeeds);
    tcase_add_test(tc, Feed_Set_Succeeds);
    tcase_add_test(tc, Feed_BufferSize_Succeeds);
    tcase_add_test(tc, Feed_Value_Fails);
    tcase_add_test(tc, Feed_Value_Succeeds);
    tcase_add_test(tc, Feed_Value_Big_Fails);

    suite_add_tcase(s, tc);

    return s;
}
