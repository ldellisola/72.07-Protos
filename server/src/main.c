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


#define null NULL
int servSock;

void SIGIOHandler(int signalType); // Handle SIGIO

int main(int argc, char ** argv) {
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("hola!");

    CliArguments arguments = ParseCli(argc,argv);


    TcpServer * tcpServer = InitTcpServer(arguments.Port);

    TcpSocket tcpSocket;

    WaitForNewConnections(tcpServer, &tcpSocket);

    do{
        char buffer[10];
        int a = ReadFromTcpSocket(&tcpSocket,buffer,10);

        if ( a == 0)
            break;

        WriteToTcpSocket(&tcpSocket,buffer,10);


        printf("%s", buffer);
    } while (true);

    return DisposeTcpServer(tcpServer);
}

