#include <stdio.h>
#include <netdb.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
#include "../headers/cli.h"
#include "../headers/tcp.h"
#include "../headers/socks.h"
#include "../headers/logger.h"


#define null NULL
int servSock;

void SIGIOHandler(int signalType); // Handle SIGIO

int main(int argc, char ** argv) {
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

        if (HandleSocks5Request(connection,buffer,bytes) == ERROR)
            break;

    } while (true);

    DisposeTcpSocket(server);
    DisposeSocks5Connection(connection);
}

