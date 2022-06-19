//
// Created by Lucas Dell'Isola on 18/06/2022.
//

#include "socks5/socks5_password_dissector.h"
#include "parsers/pop3/pop3_auth_parser.h"

#define POP3_PORT 110
static bool arePasswordsDissectorsRunning = true;


bool CanDetectPasswords(Socks5Connection * connection){
    if (!ArePasswordDissectorsEnabled() || null == connection || null == connection->RemoteTcpConnection)
        return false;

    return POP3_PORT == connection->RemotePort;
}

void EnablePasswordDissector(bool enable) {
    arePasswordsDissectorsRunning = enable;
}

bool ArePasswordDissectorsEnabled() {
    return arePasswordsDissectorsRunning;
}

bool ScanForPOP3Passwords(byte *data, size_t dataLength, Pop3AuthParser *parser, char **username, char **password) {

    ConsumePop3AuthParser(data,dataLength,parser);

    if (HasPasswordPop3AuthParser(parser)){
        *username = parser->User;
        *password = parser->Password;
        return true;
    }


    return false;
}

