//
// Created by Lucas Dell'Isola on 07/06/2022.
//

#include <check.h>
#include "lulu_client_test.h"

Suite *RegisterTcpTestSuit() {
    Suite *s = suite_create("lulu_client");
    TCase *tc = tcase_create("lulu_client");

//    tcase_add_test(tc, parser_hello);
    suite_add_tcase(s, tc);

    return s;
}
