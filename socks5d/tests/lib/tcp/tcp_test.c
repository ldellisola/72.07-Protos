//
// Created by Lucas Dell'Isola on 07/06/2022.
//

#include "tcp_test.h"

Suite *RegisterTcpTestSuit() {
    Suite *s = suite_create("tcp");
    TCase *tc = tcase_create("tcp");

//    tcase_add_test(tc, parser_hello);
    suite_add_tcase(s, tc);

    return s;
}
