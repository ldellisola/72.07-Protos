//
// Created by Lucas Dell'Isola on 19/06/2022.
//

#include "object_pool_tests.h"





Suite *RegisterObjectPoolTestSuit() {
    Suite *s = suite_create("object pool");
    TCase *tc = tcase_create("object pool");

//    tcase_add_test(tc, parser_hello);
    suite_add_tcase(s, tc);

    return s;
}
