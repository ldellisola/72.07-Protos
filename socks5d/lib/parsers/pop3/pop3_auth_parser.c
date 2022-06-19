//
// Created by Lucas Dell'Isola on 18/06/2022.
//

#include "parsers/pop3/pop3_auth_parser.h"
#include "utils/logger.h"
#include <memory.h>
#include <ctype.h>

typedef enum {
    INITIAL_STATE,
    USER_COMMAND,
    USER_CONTENT,
    PASS_COMMAND,
    PASS_CONTENT,
    UNKNOWN_COMMAND,
}States;

void ResetPop3AuthParser(Pop3AuthParser *parser) {
    if (null == parser)
    {
        Error( "POP3 Parser cannot be null");
        return;
    }

    if (null != parser->Password)
        free(parser->Password);

    if (null != parser->User)
        free(parser->User);

    memset(parser,0, sizeof(Pop3AuthParser));
    parser->State = INITIAL_STATE;
}

const char * userCommand = "user";
const char * passCommand = "pass";
const int commandLen = 4;

int FeedPop3AuthParser(byte c, Pop3AuthParser *parser) {

    if (null == parser){
        Error("POP3 Parser cannot be null");
        return FUNCTION_ERROR;
    }


    switch (parser->State) {
        case INITIAL_STATE:
            if (userCommand[parser->Index] == tolower(c))
                parser->State = USER_COMMAND;
            else if (passCommand[parser->Index] == tolower(c))
                parser->State = PASS_COMMAND;
            else
                parser->State = UNKNOWN_COMMAND;

            parser->Index++;

            break;
        case USER_COMMAND:
            if (commandLen == parser->Index && ' ' == tolower(c)) {
                parser->Index = 0;
                parser->State = USER_CONTENT;
                if (null != parser->User)
                {
                    parser->UserLength = 0;
                    free(parser->User);
                    parser->User = null;
                    parser->UserMaxSize = 0;
                }
                break;
            }

            if (userCommand[parser->Index++] != tolower(c)) {
                parser->Index = 0;
                parser->State = '\n' == c ? INITIAL_STATE : UNKNOWN_COMMAND;
            }
            break;
        case USER_CONTENT:
            if (null == parser->User) {
                parser->UserMaxSize = 100;
                parser->User = calloc(parser->UserMaxSize, sizeof(char));
            }

            if (parser->UserMaxSize == parser->UserLength){
                parser->UserMaxSize += 100;
                parser->User = realloc(parser->User,parser->UserMaxSize);
            }

            if ('\r' == c || '\n' == c) {
                parser->User[parser->UserLength] = 0;
                parser->State = '\n' == c ? INITIAL_STATE : UNKNOWN_COMMAND;
                break;
            }

            parser->User[parser->UserLength++] = c;
            break;
        case PASS_COMMAND:
            parser->IsPasswordComplete = false;

            if (commandLen == parser->Index && ' ' == tolower(c)) {
                parser->Index = 0;
                parser->State = PASS_CONTENT;
                if (null != parser->Password)
                {
                    parser->PasswordLength = 0;
                    free(parser->Password);
                    parser->Password = null;
                    parser->PasswordMaxSize = 0;
                }
                break;
            }

            if (passCommand[parser->Index++] != tolower(c)){
                parser->Index = 0;
                parser->State = '\n' == c ? INITIAL_STATE : UNKNOWN_COMMAND;
            }
            break;
        case PASS_CONTENT:
            if (null == parser->Password) {
                parser->PasswordMaxSize = 100;
                parser->Password = calloc(parser->PasswordMaxSize, sizeof(char));
            }

            if (parser->PasswordMaxSize == parser->PasswordLength){
                parser->PasswordMaxSize += 100;
                parser->Password = realloc(parser->Password,parser->PasswordMaxSize);
            }

            if ('\r' == c || '\n' == c) {
                parser->Password[parser->PasswordLength] = 0;
                parser->State = '\n' == c ? INITIAL_STATE : UNKNOWN_COMMAND;
                parser->IsPasswordComplete = true;
                break;
            }

            parser->Password[parser->PasswordLength++] = c;

            break;
        case UNKNOWN_COMMAND:

            if ('\n' == c) {
                parser->Index = 0;
                parser->State = INITIAL_STATE;
            }
            break;
    }


    return parser->State;
}

size_t ConsumePop3AuthParser(byte *data, size_t dataLength, Pop3AuthParser *parser) {
    LogDebug("Pop3AuthParser consuming %d bytes", dataLength);
        if (null == parser) {
            Error( "Cannot consume if Pop3AuthParser is NULL");
            return 0;
        }

        if (null == data) {
            Error( "Pop3AuthParser cannot consume NULL array");
            return 0;
        }

        for (size_t i = 0; i < dataLength; ++i)
            FeedPop3AuthParser(data[i],parser);
        return dataLength;
}

bool HasPasswordPop3AuthParser(Pop3AuthParser *parser) {
    if (null == parser){
        Error("POP3 Parser cannot be null");
        return false;
    }
    return parser->IsPasswordComplete;
}











