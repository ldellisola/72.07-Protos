#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <signal.h>
#include "server/cli.h"
#include "tcp/tcp.h"
#include "socks5/socks5_server.h"
#include "utils/logger.h"


static void sigterm_handler(const int signal) {
    printf("Signal %d, cleaning up and exiting\n", signal);
    StopTcpServer();
}

int main(int argc, char **argv) {
    close(0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    SetLogLevel(LOG_INFO);

    signal(SIGTERM, sigterm_handler);
    signal(SIGINT, sigterm_handler);

    CliArguments arguments = ParseCli(argc, argv);

    if (InitTcpServer(null, 0) && RegisterSocks5Server(arguments.SocksPort, arguments.SocksAddress))
        RunTcpServer();

    CleanTcpConnectionPool();

    // TODO Handle memory

}

