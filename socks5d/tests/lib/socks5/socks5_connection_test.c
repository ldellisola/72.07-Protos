//
// Created by Lucas Dell'Isola on 07/06/2022.
//

#include <check.h>
#include "socks5_connection_test.h"

Suite * RegisterSocks5ConnectionTestSuit(){
    Suite *s   = suite_create("socks5");
    TCase *tc  = tcase_create("socks5");

//    tcase_add_test(tc, parser_hello);
    suite_add_tcase(s, tc);

    return s;
}
