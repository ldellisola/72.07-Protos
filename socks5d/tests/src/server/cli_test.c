//
// Created by Lucas Dell'Isola on 07/06/2022.
//

#include <stdbool.h>
#include "cli_test.h"

START_TEST(other_test){

    ck_assert_int_eq(1,1);
}
END_TEST

Suite * RegisterCliTestSuit(){
    Suite *s   = suite_create("cli");
    TCase *tc  = tcase_create("cli");

    tcase_add_test(tc, other_test);
    suite_add_tcase(s, tc);

    return s;
}
