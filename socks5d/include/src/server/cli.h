//
// Created by Lucas Dell'Isola on 28/05/2022.
//

#ifndef SERVER_CLI_H
#define SERVER_CLI_H

#include <stdbool.h>

typedef struct {
    char Username[51];
    char Password[51];
}User;

typedef struct {
    const char * SocksPort;
    const char * SocksAddress;
    bool PrintHelp;
    bool EnablePasswordScanners;
    const char * LuluPort;
    const char * LuluAddress;
    bool PrintVersion;
    User Users[10];
    int UsersCount;
} CliArguments;

// TODO:Test

CliArguments ParseCli(int argc, char** argv);
void PrintHelp();
void PrintCLI(CliArguments arguments);



#endif //SERVER_CLI_H
