//
// Created by Lucas Dell'Isola on 07/06/2022.
//

#include <check.h>
#include <stdlib.h>
#include "parsers/hello_parser_test.h"
#include "parsers/auth_parser_test.h"
#include "parsers/request_parser_test.h"
#include "src/server/cli_test.h"
#include "lib/socks5/socks5_messages_test.h"
#include "parsers/client_hello_parser_test.h"
#include "parsers/client_goodbye_parser_test.h"
#include "parsers/client_timeout_parser_test.h"
#include "parsers/client_get_timeout_parser_test.h"
#include "parsers/client_set_buffersize_parser_test.h"
#include "parsers/client_get_buffersize_parser_test.h"
#include "parsers/client_metrics_parser_test.h"
#include "parsers/client_set_user_parser_test.h"


int main(void) {

    Suite *suites[] = {
            RegisterHelloParserTestSuit(),
            RegisterAuthParserTestSuit(),
            RegisterRequestParserTestSuit(),
            RegisterCliTestSuit(),
            RegisterSocks5MessagesTestSuit(),
            RegisterClientHelloParserTestSuit(),
            RegisterClientGoodbyeParserTestSuit(),
            RegisterClientTimeoutParserTestSuit(),
            RegisterClientGetTimeoutParserTestSuit(),
            RegisterClientSetBufferSizeParserTestSuit(),
            RegisterClientGetBufferSizeParserTestSuit(),
            RegisterClientMetricsParserTestSuit(),
            RegisterClientSetUserParserTestSuit(),
            NULL
    };
    int failedTests = 0;
    int currentSuit = 0;

    while (NULL != suites[currentSuit]) {
        SRunner *sr = srunner_create(suites[currentSuit]);
        srunner_run_all(sr, CK_NORMAL);
        failedTests += srunner_ntests_failed(sr);
        srunner_free(sr);
        currentSuit++;
    }


    return (failedTests == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
