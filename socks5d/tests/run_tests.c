//
// Created by Lucas Dell'Isola on 07/06/2022.
//

#include <check.h>
#include <stdlib.h>
#include "parsers/socks5/hello_parser_test.h"
#include "parsers/socks5/auth_parser_test.h"
#include "parsers/socks5/request_parser_test.h"
#include "lib/socks5/socks5_messages_test.h"
#include "parsers/lulu/client_hello_parser_test.h"
#include "parsers/lulu/client_goodbye_parser_test.h"
#include "parsers/lulu/client_timeout_parser_test.h"
#include "parsers/lulu/client_get_timeout_parser_test.h"
#include "parsers/lulu/client_set_buffersize_parser_test.h"
#include "parsers/lulu/client_get_buffersize_parser_test.h"
#include "parsers/lulu/client_metrics_parser_test.h"
#include "parsers/lulu/client_set_user_parser_test.h"
#include "parsers/lulu/client_list_users_parser_test.h"
#include "parsers/lulu/client_del_user_parser_test.h"
#include "parsers/pop3/pop3_auth_parser_test.h"


int main(void) {
    Suite *suites[] = {
            RegisterHelloParserTestSuit(),
            RegisterAuthParserTestSuit(),
            RegisterRequestParserTestSuit(),
            RegisterSocks5MessagesTestSuit(),
            RegisterClientHelloParserTestSuit(),
            RegisterClientGoodbyeParserTestSuit(),
            RegisterClientTimeoutParserTestSuit(),
            RegisterClientGetTimeoutParserTestSuit(),
            RegisterClientSetBufferSizeParserTestSuit(),
            RegisterClientGetBufferSizeParserTestSuit(),
            RegisterClientMetricsParserTestSuit(),
            RegisterClientSetUserParserTestSuit(),
            RegisterClientListUsersParserTestSuit(),
            RegisterClientDelUserParserTestSuit(),
            RegisterPop3AuthParserTests(),
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
