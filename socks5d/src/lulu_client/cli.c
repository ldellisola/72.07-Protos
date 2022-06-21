//
// Created by Lucas Dell'Isola on 20/06/2022.
//

#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
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
            case 'h':
                PrintHelp();
                exit(0);
            case 'u':
                p = strchr(optarg, ':');
                if(null == p) {
                    Fatal("Password not found");
                } else {

                    *p = 0;
                    p++;
                    if(strlen(p) > 255 || strlen(optarg)> 255){
                        Fatal("Password not found");
                    }else{
                        arguments.Username = optarg;
                        arguments.Password = p;
                    }
                }
                break;
            case 'L':
                arguments.Address = optarg;
                

                break;
            case 'P':
                arguments.Port = (int) strtol(optarg,null,10);
                if ((0 == arguments.Port && (EINVAL==errno || ERANGE == errno)) || (arguments.Port < 0))
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
    printf("Version 0.0.1\n");
}

void PrintHelp() {
    printf("LULU\n");
    printf("OPCIONES\n");
    printf("\t-h     Imprime la ayuda y termina.\n\n");
    printf("\t-L dirección-de-management\n\t\tEstablece la dirección donde servirá el servicio de management.\n\t\tPor defecto escucha únicamente en loopback.\n\n");
    printf("\t-P puerto-conf\n\t\tPuerto SCTP  donde escuchará por conexiones entrante del protocolo\n\t\tde configuración. Por defecto el valor es 8080.\n\n");
    printf("\t-u user:pass\n\t\tDeclara un usuario del server LULU con su contraseña.\n\n");
    printf("\t-v     Imprime información sobre la versión y termina.\n\n");
}

