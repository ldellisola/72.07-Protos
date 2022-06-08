//
// Created by Lucas Dell'Isola on 08/06/2022.
//

#include <strings.h>
#include "socks5_messages_test.h"
#include "utils/utils.h"
#include "socks5/socks5.h"
#include "socks5/socks5_messages.h"

int bufferSize = 100;
byte buffer[100];

/**************************************************************************
 *                      BuildHelloResponse Tests
 ***************************************************************************/

START_TEST(BuildHello_Succeeds){
    // Arrange
    int authenticationMethod = 0x01;
    // Act
    int size = BuildHelloResponse(buffer,bufferSize,authenticationMethod);
    // Assert
        ck_assert_int_eq(size,2);
        ck_assert_int_eq(buffer[0],SOCKS5_PROTOCOL_VERSION);
        ck_assert_int_eq(buffer[1],authenticationMethod);
}
END_TEST

/**************************************************************************
 *                      BuildAuthResponse Tests
 ***************************************************************************/

START_TEST(BuildAuth_LoggedIn_Success){
    // Arrange
    bool authenticationSucceeded = true;
    // Act
    int size = BuildAuthResponse(buffer,bufferSize,authenticationSucceeded);
    // Assert
        ck_assert_int_eq(size,2);
        ck_assert_int_eq(buffer[0],SOCKS5_PROTOCOL_VERSION);
        ck_assert_int_eq(buffer[1],SOCKS5_AUTH_SUCCESS);
}
END_TEST

START_TEST(BuildAuth_InvalidUser_Success){
    // Arrange
    bool authenticationSucceeded = false;
    // Act
    int size = BuildAuthResponse(buffer,bufferSize,authenticationSucceeded);
    // Assert
        ck_assert_int_eq(size,2);
        ck_assert_int_eq(buffer[0],SOCKS5_PROTOCOL_VERSION);
        ck_assert_int_eq(buffer[1],SOCKS5_AUTH_FAILED);
}
END_TEST

/**************************************************************************
 *                      BuildRequestResponse Tests
 ***************************************************************************/

START_TEST(BuildRequest_IPv4_Success){
    // Arrange
    byte port[] = {0x01,0x02};
    byte address[] = {127,0,0,1};
    int reply = SOCKS5_REPLY_SUCCEEDED;
    // Act
    int size = BuildRequestResponseWithIPv4(buffer,bufferSize,reply,address,port);
    // Assert
        ck_assert_int_eq(size,10);
        ck_assert_int_eq(buffer[0],SOCKS5_PROTOCOL_VERSION);
        ck_assert_int_eq(buffer[1],reply);
        ck_assert_int_eq(buffer[2],0);
        ck_assert_int_eq(buffer[3],SOCKS5_ADDRESS_TYPE_IPV4);
        ck_assert_int_eq(buffer[4],address[0]);
        ck_assert_int_eq(buffer[5],address[1]);
        ck_assert_int_eq(buffer[6],address[2]);
        ck_assert_int_eq(buffer[7],address[3]);
        ck_assert_int_eq(buffer[8],port[0]);
        ck_assert_int_eq(buffer[9],port[1]);
}
END_TEST

START_TEST(BuildRequest_IPv6_Success){
    // Arrange
        byte port[] = {0x01,0x02};
        byte address[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
        int reply = SOCKS5_REPLY_SUCCEEDED;
    // Act
    int size = BuildRequestResponseWithIPv6(buffer,bufferSize,reply,address,port);
    // Assert
        ck_assert_int_eq(size,22);
        ck_assert_int_eq(buffer[0],SOCKS5_PROTOCOL_VERSION);
        ck_assert_int_eq(buffer[1],reply);
        ck_assert_int_eq(buffer[2],0);
        ck_assert_int_eq(buffer[3],SOCKS5_ADDRESS_TYPE_IPV6);
        for (int i = 0; i < 16 ; ++i)
            ck_assert_int_eq(buffer[4+i],address[i]);
        ck_assert_int_eq(buffer[20],port[0]);
        ck_assert_int_eq(buffer[21], port[1]);
}
END_TEST

START_TEST(BuildRequest_FQDN_Success){
    // Arrange
    int reply = SOCKS5_REPLY_SUCCEEDED;
    const char * address = "www.foo.com";
    int addressLen = 11;
    byte port[] ={0x01,0x02};
    // Act
    int size = BuildRequestResponseWithFQDN(buffer,bufferSize,reply,address,port);
    // Assert
        ck_assert_int_eq(size,18);
        ck_assert_int_eq(buffer[0],SOCKS5_PROTOCOL_VERSION);
        ck_assert_int_eq(buffer[1],reply);
        ck_assert_int_eq(buffer[2],0);
        ck_assert_int_eq(buffer[3],SOCKS5_ADDRESS_TYPE_FQDN);
        ck_assert_int_eq(buffer[4], addressLen);
        for (int i = 0; i < addressLen; ++i)
            ck_assert_int_eq(buffer[5+i],address[i]);
        ck_assert_int_eq(buffer[5 + addressLen],port[0]);
        ck_assert_int_eq(buffer[5 + addressLen+1],port[1]);

}
END_TEST



void ClearBuffer(){
    bzero(buffer,bufferSize);
}

Suite *RegisterSocks5MessagesTestSuit() {
    Suite *s   = suite_create("socks5 messages");

    TCase *tc2 = tcase_create("socks5 messages");
    tcase_add_checked_fixture(tc2, ClearBuffer, ClearBuffer);

    tcase_add_test(tc2, BuildHello_Succeeds);
    tcase_add_test(tc2, BuildAuth_LoggedIn_Success);
    tcase_add_test(tc2, BuildAuth_InvalidUser_Success);
    tcase_add_test(tc2, BuildRequest_IPv4_Success);
    tcase_add_test(tc2, BuildRequest_IPv6_Success);
    tcase_add_test(tc2, BuildRequest_FQDN_Success);

    suite_add_tcase(s, tc2);

    return s;
}
