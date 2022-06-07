#include <stdio.h>
#include <memory.h>
#include <stdbool.h>
#include <unistd.h>
#include "server/cli.h"
#include "tcp/tcp.h"
#include "socks5/socks5_server.h"
#include "utils/logger.h"



int main(int argc, char ** argv) {
    close(0);
    setvbuf(stdout, NULL, _IONBF, 0);
    SetLogLevel(LOG_INFO);

    CliArguments arguments = ParseCli(argc,argv);

    if (arguments.PrintVersion) {
        printf("Version: 0.0.1");
        return 0;
    }

    if (arguments.PrintHelp){
        PrintHelp();
        return 0;
    }

    TcpSocket * server = InitSocks5Server(arguments.SocksPort);

    Socks5Connection * connection = WaitForNewSocks5Connections(server);

    do{
        byte buffer[1000];
        size_t bytes = ReadFromTcpSocket(connection->Socket,buffer,1000);
        if (bytes <= 0)
            break;

        if (RunSocks5(connection,buffer,bytes))
            break;

    } while (true);

    DisposeTcpSocket(server);
    Socks5ConnectionDestroy(connection);
}

