//
// Created by Lucas Dell'Isola on 07/06/2022.
//

#include <check.h>
#include "hello_parser_test.h"


START_TEST(parser_hello){
        ck_assert_int_eq(10, 10);
}
END_TEST


Suite * RegisterHelloParserTestSuit(){
        Suite *s   = suite_create("HelloParser");
        TCase *tc  = tcase_create("HelloParser");

        tcase_add_test(tc, parser_hello);
        suite_add_tcase(s, tc);

        return s;
}
