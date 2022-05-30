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


    TcpSocket * server = InitSocks5Server(arguments.Port);

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





//
//    Sock * tcpServer = InitTcpServer(arguments.Port);
//
//    TcpSocket * tcpSocket = WaitForNewConnections(tcpServer);
//
//    do{
//        char buffer[10];
//        int a = ReadFromTcpSocket(&tcpSocket,buffer,10);
//
//        if ( a == 0)
//            break;
//
//        WriteToTcpSocket(&tcpSocket,buffer,10);
//
//
//        printf("%s", buffer);
//    } while (true);
//
//    return DisposeTcpServer(tcpServer);
}

