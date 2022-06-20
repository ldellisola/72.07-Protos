//
// Created by tluci on 17/6/2022.
//

#include "client_get_buffersize_parser_test.h"
#include "parsers/lulu/client_get_buffersize_parser.h"
static ClientGetBufferSizeParser parser;

/**************************************************************************
 *                      ClientGetBufferSizeParserReset Tests
 ***************************************************************************/
START_TEST(Reset_Succeeds)
{
        // Arrange
        // Act
        ClientGetBufferSizeParserReset(&parser);

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
    ck_assert_int_eq(parser.Get[0], 'G');
    ck_assert_int_eq(parser.Get[1], 'E');
    ck_assert_int_eq(parser.Get[2], 'T');
    ck_assert_int_eq(parser.Get[3], 0);
    ck_assert_int_eq(parser.State, BufferSizeGet);
    ck_assert_int_eq(parser.Index, 0);
}
END_TEST
///**************************************************************************
/// *                      GetBufferSizeParserFeed Tests
/// ***************************************************************************/

START_TEST(Feed_NullParser_Fails)
{
    // Arrange
    ClientGetBufferSizeParser *nullParser = null;
    // Act
    ClientGetBufferSizeParserState state = ClientGetBufferSizeParserFeed(nullParser, 0);
    // Assert
    ck_assert_int_eq(state, BufferSizeInvalidState);
}
END_TEST

START_TEST(Feed_Complete_Succeeds)
{

    // Arrange
    ClientGetBufferSizeParserReset(&parser);
    byte message[] = {'G','E', 'T', '|','B','U','F','F','E','R','S','I','Z','E','\r', '\n'};
    ClientGetBufferSizeParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != BufferSizeInvalidState; i++ ){
        state = ClientGetBufferSizeParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, BufferSizeFinished);
}
END_TEST

START_TEST(Feed_ExtraChar_Get_Fails)
{

    // Arrange
    ClientGetBufferSizeParserReset(&parser);
    byte message[] = {'G','E', 'T', '2','|','B','U','F','F','E','R','S','I','Z','E','|','5', '6', '4','\r', '\n'};
    ClientGetBufferSizeParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != BufferSizeInvalidState; i++ ){
        state = ClientGetBufferSizeParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, BufferSizeInvalidState);
}
END_TEST

START_TEST(Feed_ExtraChar_BufferSize_Fails)
{

    // Arrange
    ClientGetBufferSizeParserReset(&parser);
    byte message[] = {'G','E', 'T', '|','B','U','F','F','E','R','S','I','Z','E','E','\r', '\n'};
    ClientGetBufferSizeParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != BufferSizeInvalidState; i++ ){
        state = ClientGetBufferSizeParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, BufferSizeInvalidState);
}
END_TEST


START_TEST(Feed_CRLF_Fails)
{

    // Arrange
    ClientGetBufferSizeParserReset(&parser);
    byte message[] = {'G','E', 'T', '|','B','U','F','F','E','R','S','I','Z','E','\r', '\r'};
    ClientGetBufferSizeParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != BufferSizeInvalidState; i++ ){
        state = ClientGetBufferSizeParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, BufferSizeInvalidState);
}
END_TEST

START_TEST(Feed_Complete_ExtraChar_Succeeds)
{

    // Arrange
    ClientGetBufferSizeParserReset(&parser);
    byte message[] = {'G','E', 'T', '|','B','U','F','F','E','R','S','I','Z','E','\r', '\n', '3'};
    ClientGetBufferSizeParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != BufferSizeInvalidState; i++ ){
        state = ClientGetBufferSizeParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, BufferSizeFinished);
}
END_TEST

START_TEST(Feed_Get_Succeeds)
{

    // Arrange
    ClientGetBufferSizeParserReset(&parser);
    byte message[] = {'G','E', 'T', '|'};
    ClientGetBufferSizeParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != BufferSizeInvalidState; i++ ){
        state = ClientGetBufferSizeParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, BufferSize);
}
END_TEST

START_TEST(Feed_Get_Fails)
{

    // Arrange
    ClientGetBufferSizeParserReset(&parser);
    byte message[] = {'G','|' };
    ClientGetBufferSizeParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != BufferSizeInvalidState; i++ ){
        state = ClientGetBufferSizeParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, BufferSizeInvalidState);
}
END_TEST

START_TEST(Feed_BufferSize_Succeeds)
{

    // Arrange
    ClientGetBufferSizeParserReset(&parser);
    byte message[] = {'G','E', 'T', '|','B','U','F','F','E','R','S','I','Z','E','\r'};
    ClientGetBufferSizeParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != BufferSizeInvalidState; i++ ){
        state = ClientGetBufferSizeParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, BufferSizeCRLF);
}
END_TEST

START_TEST(Feed_BufferSize_Fails)
{

    // Arrange
    ClientGetBufferSizeParserReset(&parser);
    byte message[] = {'G','E', 'T', '|','B','U','F','F','E','R','S','I','Z','o','\r'};
    ClientGetBufferSizeParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != BufferSizeInvalidState; i++ ){
        state = ClientGetBufferSizeParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, BufferSizeInvalidState);
}
END_TEST



Suite *RegisterClientGetBufferSizeParserTestSuit() {
    Suite *s = suite_create("GetBufferSizeClientParser");

    TCase *tc = tcase_create("GetBufferSizeClientParser");
    tcase_add_test(tc, Reset_Succeeds);
    tcase_add_test(tc, Feed_NullParser_Fails);
    tcase_add_test(tc, Feed_Complete_ExtraChar_Succeeds );
    tcase_add_test(tc, Feed_CRLF_Fails);
    tcase_add_test(tc, Feed_Complete_Succeeds);
    tcase_add_test(tc, Feed_ExtraChar_Get_Fails);
    tcase_add_test(tc, Feed_ExtraChar_BufferSize_Fails);
    tcase_add_test(tc, Feed_Get_Succeeds);
    tcase_add_test(tc, Feed_BufferSize_Succeeds);
    tcase_add_test(tc, Feed_BufferSize_Fails);
    tcase_add_test(tc, Feed_Get_Fails);

    suite_add_tcase(s, tc);

    return s;
}
