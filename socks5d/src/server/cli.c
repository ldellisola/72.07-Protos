//
// Created by Lucas Dell'Isola on 28/05/2022.
//

#include <string.h>
#include <stdio.h>
#include <strings.h>
#include "server/cli.h"
#include "utils/utils.h"
#include "utils/logger.h"

bool HasArgument(int argc, char **argv, const char *argument);

const char *GetSingleArgumentValue(int argc, char **argv, const char *argument);

const char *GetMultipleArgumentValue(int argc, char **argv, int *startIndex, const char *argument);

void PrintVersion();

CliArguments ParseCli(int argc, char **argv) {
    CliArguments args = {
            .SocksPort="1080",
            .SocksAddress = null,
            .EnablePasswordScanners=true,
            .LuluPort="8080",
            .LuluAddress="localhost",
            .UsersCount=0,
    };

    bool printHelp = HasArgument(argc, argv, "-h");
    if (printHelp) {
        LogInfo("Printing Help Command");
        PrintHelp();
        exit(0);
    }

    const char *socks5Address = GetSingleArgumentValue(argc, argv, "-l");
    if (null != socks5Address) {
        args.SocksAddress = socks5Address;
        LogInfo("Using SOCKS5 ADDRESS %s", args.SocksAddress);
    } else
        LogInfo("Using SOCKS5 ADDRESS all interfaces");

    bool enablePasswordScanners = !HasArgument(argc, argv, "-N");
    if (enablePasswordScanners)
        LogInfo("Password scanners enabled");
    else {
        args.EnablePasswordScanners = false;
        LogInfo("Password scanners disabled");
    }

    const char *luluAddress = GetSingleArgumentValue(argc, argv, "-L");
    if (null != luluAddress) {
        LogInfo("Using LULU ADDRESS %s", luluAddress);
        args.LuluAddress = luluAddress;
    } else
        LogInfo("Using default LULU ADDRESS %s", args.LuluAddress);


    const char *socks5Port = GetSingleArgumentValue(argc, argv, "-p");
    if (null != socks5Port) {
        args.SocksPort = socks5Port;
        LogInfo("Using SOCKS5 PORT %s", args.SocksPort);
    } else
        LogInfo("Using default SOCKS5 PORT %s", args.SocksPort);


    const char *luluPort = GetSingleArgumentValue(argc, argv, "-P");
    if (null != luluPort) {
        LogInfo("Using LULU PORT %s", luluPort);
        args.LuluPort = luluPort;
    } else
        LogInfo("Using default LULU PORT %s", args.LuluPort);

    int start = 0;
    do {
        const char *userData = GetMultipleArgumentValue(argc, argv, &start, "-u");
        if (null == userData)
            continue;
        char *dividerAdds = strchr(userData, ':');
        if (null == dividerAdds)
            continue;
        int dividerPos = dividerAdds - userData;

        User user;
        bzero(user.Username, 51);
        strncpy(user.Username, userData, dividerPos);
        bzero(user.Password, 51);
        strncpy(user.Password, userData + dividerPos + 1, strlen(userData) - dividerPos - 1);
        args.Users[args.UsersCount++] = user;
        LogInfo("Detecting user %s:%s", user.Username, user.Password);

    } while (start < argc - 1 && args.UsersCount < 10);

    bool printVersion = HasArgument(argc, argv, "-v");
    if (printVersion) {
        LogInfo("Printing version");
        PrintVersion();
        exit(0);
    }


    return args;
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

void PrintCLI(CliArguments arguments) {

}

bool HasArgument(int argc, char **argv, const char *argument) {

    for (int i = 0; i < argc; ++i) {
        if (0 == strcmp(argument, argv[i]))
            return true;
    }

    return false;
}

const char *GetSingleArgumentValue(int argc, char **argv, const char *argument) {
    int start = 0;
    return GetMultipleArgumentValue(argc, argv, &start, argument);
}

const char *GetMultipleArgumentValue(int argc, char **argv, int *startIndex, const char *argument) {
    while (*startIndex < argc - 1) {
        int i = (*startIndex)++;
        if (0 == strcmp(argument, argv[i]))
            return argv[i + 1];
    }
    return null;
}
