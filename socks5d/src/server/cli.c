//
// Created by Lucas Dell'Isola on 28/05/2022.
//

#include <string.h>
#include <stdio.h>
#include <strings.h>
#include <getopt.h>
#include <errno.h>
#include "server/cli.h"
#include "utils/utils.h"
#include "utils/logger.h"


void PrintVersion();
void PrintHelp();

void LoadUser(CliArguments * args,int userNum, char * user);

CliArguments ParseCli(int argc, char **argv) {
    CliArguments args = {
            .SocksPort="1080",
            .SocksAddress = null,
            .EnablePasswordScanners=true,
            .LuluPort="8080",
            .LuluAddress=null,
            .BufferSize = 512,
            .Timeout = 50
    };

    memset(args.Usernames,0,sizeof(args.Usernames));
    memset(args.Passwords,0,sizeof(args.Passwords));

    int numberOfUsers = 0;
    int ch;
    long value;
    while ((ch = getopt(argc,argv,"hl:L:Np:P:u:vb:t:")) != -1){
        switch (ch) {
            case 'l':
                args.SocksAddress = optarg;
                break;
            case 'N':
                args.EnablePasswordScanners = false;
                break;
            case 'L':
                args.LuluAddress = optarg;
                break;
            case 'p':
                args.SocksPort = optarg;
                break;
            case 'P':
                args.LuluPort = optarg;
                break;
            case 'b':
                value = strtol(optarg, null, 10);
                if (0 >= value)
                    Fatal("Invalid buffer size");
                else
                    args.BufferSize = value;
                break;
            case 't':
                value = strtol(optarg, null, 10);
                if (0  == value && (EINVAL == errno || ERANGE == errno))
                    Fatal("Invalid timeout value");
                else
                    args.Timeout = value;
                break;
            case 'u':
                if (numberOfUsers < 10)
                    LoadUser(&args, numberOfUsers++,optarg);
                else {
                    Fatal("Invalid number of users detected. Maximum is 10");
                }
                break;
            case 'v':
                PrintVersion();
                exit(0);
            case 'h':
            default:
                PrintHelp();
                exit(0);
        }

    }

    return args;
}


void LoadUser(CliArguments * args,int userNum, char * user){
    char *p = strchr(user, ':');
    if(null == p) {
        Error("Password not found");
        exit(1);
    } else {
        *p = 0;
        p++;
        args->Usernames[userNum] = user;
        args->Passwords[userNum] = p;
    }
}

void PrintVersion() {
    printf("Version 0.0.1");
}


void PrintHelp() {
    printf("socks5d [ POSIX style options ]\n");
    printf("OPCIONES\n");
    printf("\t-h     Imprime la ayuda y termina.\n\n");
    printf("\t-l dirección-socks\n\t\tEstablece la dirección donde servirá el proxy SOCKS.Por defecto\n\t\tescucha en todas las interfaces.\n\n");
    printf("\t-N     Deshabilita los passwords disectors.\n\n");
    printf("\t-L dirección-de-management\n\t\tEstablece la dirección donde servirá el servicio de management.\n\t\tPor defecto escucha únicamente en loopback.\n\n");
    printf("\t-p puerto-local\n\t\tPuerto TCP donde escuchará por conexiones entrantes SOCKS.  Por\n\t\tdefecto el valor es 1080.\n\n");
    printf("\t-P puerto-conf\n\t\tPuerto SCTP  donde escuchará por conexiones entrante del protocolo\n\t\tde configuración. Por defecto el valor es 8080.\n\n");
    printf("\t-u user:pass\n\t\tDeclara un usuario del proxy con su contraseña. Se puede utilizar\n\t\thasta 10 veces.\n\n");
    printf("\t-v     Imprime información sobre la versión versión y termina.\n\n");
}

