//
// Created by tluci on 17/6/2022.
//

#include "client_metrics_parser_test.h"
#include "parsers/lulu/client_metrics_parser.h"
ClientMetricsParser parser;

/**************************************************************************
 *                      ClientMetricsParserReset Tests
 ***************************************************************************/
START_TEST(Reset_Succeeds)
{
        // Arrange
        // Act
        ClientMetricsParserReset(&parser);

        // Assert
        ck_assert_int_eq(parser.Metrics[0], 'M');
        ck_assert_int_eq(parser.Metrics[1], 'E');
        ck_assert_int_eq(parser.Metrics[2], 'T');
        ck_assert_int_eq(parser.Metrics[3], 'R');
        ck_assert_int_eq(parser.Metrics[4], 'I');
        ck_assert_int_eq(parser.Metrics[5], 'C');
        ck_assert_int_eq(parser.Metrics[6], 'S');
        ck_assert_int_eq(parser.Metrics[7], 0);

        ck_assert_int_eq(parser.Get[0], 'G');
        ck_assert_int_eq(parser.Get[1], 'E');
        ck_assert_int_eq(parser.Get[2], 'T');
        ck_assert_int_eq(parser.Get[3], 0);
        ck_assert_int_eq(parser.State, MetricsGet);
        ck_assert_int_eq(parser.Index, 0);
        }
END_TEST
///**************************************************************************
/// *                      MetricsParserFeed Tests
/// ***************************************************************************/

START_TEST(Feed_NullParser_Fails)
{
    // Arrange
    ClientMetricsParser *nullParser = null;
    // Act
    ClientMetricsParserState state = ClientMetricsParserFeed(nullParser, 0);
    // Assert
    ck_assert_int_eq(state, MetricsInvalidState);
}
END_TEST

START_TEST(Feed_Complete_Succeeds)
{

    // Arrange
    ClientMetricsParserReset(&parser);
    byte message[] = {'G','E', 'T', '|','M','E','T','R','I','C','S','\r', '\n'};
    ClientMetricsParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != MetricsInvalidState; i++ ){
        state = ClientMetricsParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, MetricsFinished);
}
END_TEST

START_TEST(Feed_ExtraChar_Get_Fails)
{

    // Arrange
    ClientMetricsParserReset(&parser);
    byte message[] = {'G','E', 'T', '0','|','M','E','T','R','I','C','S','\r'};
    ClientMetricsParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != MetricsInvalidState; i++ ){
        state = ClientMetricsParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, MetricsInvalidState);
}
END_TEST

START_TEST(Feed_ExtraChar_Metrics_Fails)
{

    // Arrange
    ClientMetricsParserReset(&parser);
    byte message[] = {'G','E', 'T', '|','M','E','E','T','R','I','C','0','S','\r'};
    ClientMetricsParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != MetricsInvalidState; i++ ){
        state = ClientMetricsParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, MetricsInvalidState);
}
END_TEST


START_TEST(Feed_CRLF_Fails)
{

    // Arrange
    ClientMetricsParserReset(&parser);
    byte message[] = {'G','E', 'T', '|','M','E','T','R','I','C','S','\r', 'd', '\n'};
    ClientMetricsParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != MetricsInvalidState; i++ ){
        state = ClientMetricsParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, MetricsInvalidState);
}
END_TEST

START_TEST(Feed_Complete_ExtraChar_Succeeds)
{

    // Arrange
    ClientMetricsParserReset(&parser);
    byte message[] = {'G','E', 'T', '|','M','E','T','R','I','C','S','\r', '\n', 3, 1};
    ClientMetricsParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != MetricsInvalidState; i++ ){
        state = ClientMetricsParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, MetricsFinished);
}
END_TEST

START_TEST(Feed_Get_Succeeds)
{

    // Arrange
    ClientMetricsParserReset(&parser);
    byte message[] = {'G','E', 'T', '|'};
    ClientMetricsParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != MetricsInvalidState; i++ ){
        state = ClientMetricsParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, Metrics);
}
END_TEST

START_TEST(Feed_Get_Fails)
{

    // Arrange
    ClientMetricsParserReset(&parser);
    byte message[] = {'G','|' };
    ClientMetricsParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != MetricsInvalidState; i++ ){
        state = ClientMetricsParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, MetricsInvalidState);
}
END_TEST

START_TEST(Feed_Metrics_Succeeds)
{

    // Arrange
    ClientMetricsParserReset(&parser);
    byte message[] = {'G','E', 'T', '|','M','E','T','R','I','C','S','\r'};
    ClientMetricsParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != MetricsInvalidState; i++ ){
        state = ClientMetricsParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, MetricsCRLF);
}
END_TEST

START_TEST(Feed_Metrics_Fails)
{

    // Arrange
    ClientMetricsParserReset(&parser);
    byte message[] = {'G','E', 'T', '|','M','E','T','R','I','C','0','S','\r'};
    ClientMetricsParserState state;
    // Act
    for (int i=0; i < (int)sizeof(message)/(int) sizeof(message[0]) && state != MetricsInvalidState; i++ ){
        state = ClientMetricsParserFeed(&parser, message[i]);
    }

    // Assert
    ck_assert_int_eq(state, MetricsInvalidState);
}
END_TEST



Suite *RegisterClientMetricsParserTestSuit() {
    Suite *s = suite_create("MetricsClientParser");

    TCase *tc = tcase_create("MetricsClientParser");
    tcase_add_test(tc, Reset_Succeeds);
    tcase_add_test(tc, Feed_NullParser_Fails);
    tcase_add_test(tc, Feed_Complete_ExtraChar_Succeeds );
    tcase_add_test(tc, Feed_CRLF_Fails);
    tcase_add_test(tc, Feed_Complete_Succeeds);
    tcase_add_test(tc, Feed_ExtraChar_Get_Fails);
    tcase_add_test(tc, Feed_ExtraChar_Metrics_Fails);
    tcase_add_test(tc, Feed_Get_Succeeds);
    tcase_add_test(tc, Feed_Metrics_Succeeds);
    tcase_add_test(tc, Feed_Metrics_Fails);
    tcase_add_test(tc, Feed_Get_Fails);

    suite_add_tcase(s, tc);

    return s;
}
