//
// Created by Lucas Dell'Isola on 28/05/2022.
//

#ifndef SERVER_CLI_H
#define SERVER_CLI_H

#include <stdbool.h>
#include "socks5/socks5.h"


typedef struct {
    const char *SocksPort;
    const char *SocksAddress;
    bool EnablePasswordScanners;
    const char *LuluPort;
    const char *LuluAddress;
    User Users[10];
    int UsersCount;
} CliArguments;

// TODO:Test

CliArguments ParseCli(int argc, char **argv);

void PrintHelp();

void PrintCLI(CliArguments arguments);


#endif //SERVER_CLI_H
