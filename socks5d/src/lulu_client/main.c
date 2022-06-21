//
// Created by Lucas Dell'Isola on 07/06/2022.
//
#include "utils/utils.h"
#include "utils/logger.h"
#include <stdio.h> 		// fgets, printf
#include <string.h> 	// memset, memc
#include <signal.h>
#include <arpa/inet.h>
#include "lulu_client/cli.h"
#include "selector/selector.h"
#include "tcp/tcp.h"
#include "lulu_client/client_data.h"


static ClientData clientData;
static fd_selector selector = NULL;

void sigterm_handler(){
    StopTcpServer();
}

static const SelectorOptions options = {
        .Signal = SIGALRM,
        .SelectTimeout = {
                .tv_sec = 1,
                .tv_nsec = 0,
        },
        .OnConnectionCall = null,
        .OnTimeout = null
};

int main(int argc, char *argv[]) {
    SetLogLevelFromEnvironment();

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    signal(SIGTERM, sigterm_handler);
    signal(SIGINT, sigterm_handler);

    CliArguments args = ParseCli(argc,argv);

    if (false == InitTcpServer(&options,2))
        Fatal("Cannot start TCP");

    selector = GetSelector();
    InitClient(&clientData);

    clientData.Username = args.Username;
    clientData.Password = args.Password;


    if (SELECTOR_STATUS_SUCCESS != SelectorRegister(selector,0,GetClientHandlers(),SELECTOR_OP_NOOP,&clientData,false))
        Fatal("Cannot register stdin as input");

    int remoteAddressFamily = GetAddressFamily(args.Address);
    struct sockaddr_storage address;

    TcpConnection * connection = null;
    switch (remoteAddressFamily) {
        case  AF_INET: {
            struct in_addr in4;
            inet_pton(AF_INET, args.Address, &in4);
            ((struct sockaddr_in *) &address)->sin_family = AF_INET;
            ((struct sockaddr_in *) &address)->sin_port = htons(args.Port);
            ((struct sockaddr_in *) &address)->sin_addr = in4;
            connection = ConnectToIPv4TcpServer((struct sockaddr *) &address, GetClientHandlers(), &clientData);
        }
            break;
        case AF_INET6: {
            struct in6_addr in6;
            inet_pton(AF_INET6, args.Address, &in6);
            ((struct sockaddr_in6 *) &address)->sin6_family = AF_INET6;
            ((struct sockaddr_in6 *) &address)->sin6_addr = in6;
            ((struct sockaddr_in6 *) &address)->sin6_port = htons(args.Port);
            connection = ConnectToIPv6TcpServer((struct sockaddr *) &address, GetClientHandlers(), &clientData);
        }
            break;
        default:
            Fatal("Invalid Address");
    }

    if (null == connection)
        LogFatalWithReason("Cannot connecto to address %s",args.Address);

    clientData.Remote = connection;

    // Run server
    RunTcpServer();

    // Dispose data

    SelectorUnregisterFd(selector,0);
    DisposeTcpServer();
    free(clientData.Buffer.Data);


    return 0;
}
