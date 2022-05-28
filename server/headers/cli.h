//
// Created by Lucas Dell'Isola on 28/05/2022.
//

#ifndef SERVER_CLI_H
#define SERVER_CLI_H

typedef struct {
    char * Port;

} CliArguments;

CliArguments ParseCli(int argc, char** argv);



#endif //SERVER_CLI_H
