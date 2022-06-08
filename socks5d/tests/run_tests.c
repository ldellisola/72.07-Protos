//
// Created by Lucas Dell'Isola on 07/06/2022.
//

#include <check.h>
#include <stdlib.h>
#include "parsers/hello_parser_test.h"
#include "parsers/auth_parser_test.h"
#include "server/cli_test.h"

int main(void) {

    Suite *suites [] = {
            RegisterHelloParserTestSuit(),
            RegisterAuthParserTestSuit(),
            RegisterCliTestSuit(),
            NULL
    };
    int failedTests = 0;
    int currentSuit = 0;

    while (NULL != suites[currentSuit]){
        SRunner *sr  = srunner_create(suites[currentSuit]);
        srunner_run_all(sr, CK_NORMAL);
        failedTests += srunner_ntests_failed(sr);
        srunner_free(sr);
        currentSuit++;
    }


    return (failedTests == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
