//
// Created by Lucas Dell'Isola on 28/05/2022.
//

#ifndef SERVER_CLI_H
#define SERVER_CLI_H

#include <stdbool.h>


typedef struct {
    const char *SocksPort;
    const char *SocksAddress;
    bool EnablePasswordScanners;
    const char *LuluPort;
    const char *LuluAddress;
    const char * Usernames[11];
    const char * Passwords[11];
    size_t BufferSize;
} CliArguments;


CliArguments ParseCli(int argc, char **argv);


#endif //SERVER_CLI_H
