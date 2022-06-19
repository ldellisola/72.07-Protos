//
// Created by Lucas Dell'Isola on 18/06/2022.
//

#include "pop3_auth_parser_test.h"
#include "parsers/pop3/pop3_auth_parser.h"
typedef enum {
    INITIAL_STATE,
    USER_COMMAND,
    USER_CONTENT,
    PASS_COMMAND,
    PASS_CONTENT,
    UNKNOWN_COMMAND,
}States;

Pop3AuthParser parser;

START_TEST(ResetParser_Succeeds){
    // Arrange
    // Act
        ResetPop3AuthParser(&parser);
    // Assert
        ck_assert_int_eq(parser.State, INITIAL_STATE);
        ck_assert_int_eq(parser.Index, 0);
        ck_assert_int_eq(parser.PasswordLength, 0);
        ck_assert_int_eq(parser.PasswordMaxSize, 0);
        ck_assert_int_eq(parser.IsPasswordComplete,false);
        ck_assert_ptr_eq(parser.Password,null);
}END_TEST

START_TEST(Consume_NullData_Fails){
        // Arrange
        void *  nullData = null;
        // Act
        size_t count = ConsumePop3AuthParser( nullData, 10,&parser);
        // Assert
        ck_assert_int_eq(count, 0);
    }END_TEST

START_TEST(Consume_NullParser_Fails){
        // Arrange
        void * nullParser = null;
        char *  data = "user";
        // Act
        size_t count = ConsumePop3AuthParser( (void * )data, strlen(data),nullParser);
        // Assert
        ck_assert_int_eq(count, 0);
    }END_TEST


START_TEST(Feed_NullParser_Fails){
    // Arrange
    void * nullParser = null;
    // Act
        int state = FeedPop3AuthParser(1,nullParser);
    // Assert
        ck_assert_int_eq(state, -1);
}END_TEST

START_TEST(Consume_InvalidCommand_Succeeds){
    // arrange
    const char * data = "QUIT";
        size_t dataLen = strlen(data);
    // act
        ConsumePop3AuthParser((void *)data,dataLen,&parser);
        // assert
        ck_assert_int_eq(parser.State, UNKNOWN_COMMAND);
}
END_TEST

START_TEST(Consume_MultipleInvalidCommand_Succeeds){
    // arrange
    const char * data = "QUIT\r\nQUIT\r\n";
        size_t dataLen = strlen(data);
    // act
        ConsumePop3AuthParser((void *)data,dataLen,&parser);
        // assert
        ck_assert_int_eq(parser.State, INITIAL_STATE);
}
END_TEST

START_TEST(Consume_InvalidUserLikeCommand_Succeeds){
    // arrange
    const char * data = "UserNOT";
        size_t dataLen = strlen(data);
    // act
        ConsumePop3AuthParser((void *)data,dataLen,&parser);
        // assert
        ck_assert_int_eq(parser.State, UNKNOWN_COMMAND);
}
END_TEST

START_TEST(Consume_InvalidPassLikeCommand_Succeeds){
    // arrange
    const char * data = "paNOTss";
        size_t dataLen = strlen(data);
    // act
        ConsumePop3AuthParser((void *)data,dataLen,&parser);
        // assert
        ck_assert_int_eq(parser.State, UNKNOWN_COMMAND);
}
END_TEST

START_TEST(Consume_UserCommand_Succeeds){
    // arrange
    const char * data = "USER";
        size_t dataLen = strlen(data);
    // act
        ConsumePop3AuthParser((void *)data,dataLen,&parser);
        // assert
        ck_assert_int_eq(parser.State, USER_COMMAND);
        ck_assert_int_eq(parser.Index, 4);
}
END_TEST

START_TEST(Consume_UserCommandWithSpace_Succeeds){
    // arrange
    const char * data = "USER ";
        size_t dataLen = strlen(data);
    // act
        ConsumePop3AuthParser((void *)data,dataLen,&parser);
        // assert
        ck_assert_int_eq(parser.State, USER_CONTENT);
}
END_TEST

START_TEST(Consume_CompleteUserOnly_Succeeds){
    // arrange
    const char * data = "USER lucas\r\n";
        size_t dataLen = strlen(data);
    // act
        ConsumePop3AuthParser((void *)data,dataLen,&parser);
        // assert
        ck_assert_int_eq(parser.State, INITIAL_STATE);
        ck_assert_int_eq(parser.Index, 0);
        ck_assert_str_eq(parser.User,"lucas");
}
END_TEST

START_TEST(Consume_PassCommand_Succeeds){
        // arrange
        const char * data = "PASS";
        size_t dataLen = strlen(data);
        // act
        ConsumePop3AuthParser((void *)data,dataLen,&parser);
        // assert
        ck_assert_int_eq(parser.State, PASS_COMMAND);
        ck_assert_int_eq(parser.Index, 4);
    }
END_TEST

START_TEST(Consume_PassCommandWithSpace_Succeeds){
        // arrange
        const char * data = "PASS ";
        size_t dataLen = strlen(data);
        // act
        ConsumePop3AuthParser((void *)data,dataLen,&parser);
        // assert
        ck_assert_int_eq(parser.State, PASS_CONTENT);
    }
END_TEST

START_TEST(Consume_CompletePass_Succeeds){
        // arrange
        const char * data = "pass lucas\r\n";
        size_t dataLen = strlen(data);
        // act
        ConsumePop3AuthParser((void *)data,dataLen,&parser);
        // assert
        ck_assert_int_eq(parser.State, INITIAL_STATE);
        ck_assert_int_eq(parser.Index, 0);
        ck_assert_str_eq(parser.Password,"lucas");
        ck_assert_int_eq(parser.IsPasswordComplete, true);
        ck_assert_int_eq(parser.PasswordLength, strlen("lucas"));
    }
END_TEST


START_TEST(Consume_CompleteLongPass_Succeeds){
        // arrange
        const char * longPass = "lucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucas";
        const char * data = "pass lucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucaslucas\r\n";
        size_t dataLen = strlen(data);
        // act
        ConsumePop3AuthParser((void *)data,dataLen,&parser);
        // assert
        ck_assert_int_eq(parser.State, INITIAL_STATE);
        ck_assert_int_eq(parser.Index, 0);
        ck_assert_str_eq(parser.Password,longPass);
        ck_assert_int_eq(parser.IsPasswordComplete, true);
        ck_assert_int_eq(parser.PasswordLength, strlen(longPass));
    }
END_TEST

START_TEST(Consume_TwoPasswords_Succeeds){
        // arrange
        const char * data = "pass lucas\r\npass juan\r\n";
        size_t dataLen = strlen(data);
        // act
        ConsumePop3AuthParser((void *)data,dataLen,&parser);
        // assert
        ck_assert_int_eq(parser.State, INITIAL_STATE);
        ck_assert_int_eq(parser.Index, 0);
        ck_assert_str_eq(parser.Password,"juan");
        ck_assert_int_eq(parser.IsPasswordComplete, true);
        ck_assert_int_eq(parser.PasswordLength, strlen("juan"));
    }
END_TEST


void ClearParser(){
    ResetPop3AuthParser(&parser);
}

Suite *RegisterPop3AuthParserTests() {
    Suite *s = suite_create("Pop3AuthParser");

    TCase *tc = tcase_create("Pop3AuthParser");
    tcase_add_test(tc, ResetParser_Succeeds);
    tcase_add_test(tc, Feed_NullParser_Fails);
    suite_add_tcase(s, tc);

    TCase *tc2 = tcase_create("Pop3AuthParser with Teardown");
    tcase_add_checked_fixture(tc2, ClearParser, ClearParser);
    tcase_add_test(tc2, Consume_UserCommand_Succeeds);
    tcase_add_test(tc2, Consume_CompleteUserOnly_Succeeds);
    tcase_add_test(tc2, Consume_UserCommandWithSpace_Succeeds);
    tcase_add_test(tc2, Consume_InvalidCommand_Succeeds);
    tcase_add_test(tc2, Consume_InvalidUserLikeCommand_Succeeds);
    tcase_add_test(tc2, Consume_InvalidPassLikeCommand_Succeeds);
    tcase_add_test(tc2, Consume_PassCommand_Succeeds);
    tcase_add_test(tc2, Consume_PassCommandWithSpace_Succeeds);
    tcase_add_test(tc2, Consume_CompletePass_Succeeds);
    tcase_add_test(tc2, Consume_TwoPasswords_Succeeds);
    tcase_add_test(tc2, Consume_CompleteLongPass_Succeeds);
    tcase_add_test(tc2, Consume_MultipleInvalidCommand_Succeeds);
    tcase_add_test(tc2, Consume_NullData_Fails);
    tcase_add_test(tc2, Consume_NullParser_Fails);
    suite_add_tcase(s, tc2);

    return s;
}
