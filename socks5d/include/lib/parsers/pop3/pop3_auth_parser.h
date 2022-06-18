//
// Created by Lucas Dell'Isola on 18/06/2022.
//

#ifndef SOCKS5D_POP3_AUTH_PARSER_H
#define SOCKS5D_POP3_AUTH_PARSER_H

#include <stdbool.h>
#include "utils/utils.h"

typedef struct {
    int State;
    int Index;
    char * User;
    int UserLength;
    int UserMaxSize;
    char * Password;
    int PasswordLength;
    int PasswordMaxSize;
    bool IsPasswordComplete;
}Pop3AuthParser;

/**
 * It restore the initial state of the parser
 * @param parser Parser to be reset
 */
void ResetPop3AuthParser(Pop3AuthParser * parser);

/**
 * It feeds one character to the Pop3AuthParser
 * @param c Character to feed
 * @param parser Parser
 * @return The current state of the parser
 */
int FeedPop3AuthParser(byte c, Pop3AuthParser * parser);

/**
 * It consumes an array of bytes and feeds it to the parser
 * @param data Data to be consume
 * @param dataLength Size of the data array
 * @param parser Parser
 * @return Number of byte consumed
 */
size_t ConsumePop3AuthParser(byte * data, size_t dataLength, Pop3AuthParser * parser);

/**
 * It checks if the parser found a password
 * @param parser Parser
 * @return Whether the password was found or not
 */
bool HasPasswordPop3AuthParser(Pop3AuthParser * parser);





#endif //SOCKS5D_POP3_AUTH_PARSER_H
