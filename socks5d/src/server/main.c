#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "server/cli.h"
#include "tcp/tcp.h"
#include "socks5/socks5_server.h"
#include "socks5/socks5_connection.h"
#include "utils/logger.h"


static void sigterm_handler(const int signal) {
    printf("Signal %d, cleaning up and exiting\n", signal);
    StopTcpServer();
}

int main(int argc, char **argv) {
    SetLogLevelFromEnvironment();

    close(0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    Info("Starting server...");

    signal(SIGTERM, sigterm_handler);
    signal(SIGINT, sigterm_handler);


    CliArguments arguments = ParseCli(argc, argv);

    SelectorOptions options = {
            .Signal = SIGALRM,
            .SelectTimeout = {
                    .tv_sec = 1,
                    .tv_nsec = 0,
            },
            .OnTimeout = CheckForTimeoutInSocks5Connections,
            .OnConnectionCall = NotifySocks5ConnectionAccess

    };
    int socks5PoolSize = 50;
    bool startServer = true;
    startServer &= InitTcpServer(&options, socks5PoolSize * 2);
    startServer &= RegisterSocks5Server(
            arguments.SocksPort,
            arguments.SocksAddress,
            socks5PoolSize,
            arguments.Timeout,
            arguments.Usernames,
            arguments.Passwords,
            arguments.EnablePasswordScanners,
            arguments.BufferSize
    );


    if (startServer) {
        RunTcpServer();
    }
    else{
        Warning("Server could not be started");
    }

    Info("Shutting down...");
    DisposeSocks5Server();
    DisposeTcpServer();
    Info("Shut down complete!");

}

