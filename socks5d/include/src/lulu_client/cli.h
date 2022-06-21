//
// Created by Lucas Dell'Isola on 20/06/2022.
//

#ifndef SOCKS5D_CLI_H
#define SOCKS5D_CLI_H

typedef struct {
    const char * Username;
    const char * Password;
    const char * Address;
    int Port;
}CliArguments;


CliArguments ParseCli(int argc, char ** argv);


#endif //SOCKS5D_CLI_H
