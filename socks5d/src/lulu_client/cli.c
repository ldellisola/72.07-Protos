//
// Created by Lucas Dell'Isola on 20/06/2022.
//

#include <getopt.h>
#include <string.h>
#include <errno.h>
#include "lulu_client/cli.h"
#include "utils/utils.h"
#include "utils/logger.h"

void PrintHelp();

void PrintVersion();

CliArguments ParseCli(int argc, char ** argv){
    CliArguments  arguments = {
            .Password = null,
            .Username = null,
            .Address = "127.0.0.1",
            .Port = 8080
    };

    int ch;
    char *p;
    while ((ch = getopt(argc,argv,"vhu:L:P:")) != -1) {
        switch (ch) {
            case 'v':
                PrintVersion();
                exit(0);
                break;
            case 'h':
                PrintHelp();
                exit(0);
                break;
            case 'u':
                p = strchr(optarg, ':');
                if(null == p) {
                    Fatal("Password not found");
                } else {
                    *p = 0;
                    p++;
                    arguments.Username = optarg;
                    arguments.Password = p;
                }
                break;
            case 'L':
                arguments.Address = optarg;
                break;
            case 'P':
                arguments.Port = (int) strtol(optarg,null,10);
                if (0 == arguments.Port && (EINVAL==errno || ERANGE == errno))
                    LogFatalWithReason("Invalid port number: %s",optarg);

                break;
            default:
                LogFatal("Invalid parameter %c",ch);
        }

    }

    // Validar puerto
    // Validar longitud de nombre y contrasena
    if (null == arguments.Username || null == arguments.Password) {
        PrintHelp();
        Fatal("Username and password are mandatory!");
    }


    return arguments;
}

void PrintVersion() {

}

void PrintHelp() {

}
