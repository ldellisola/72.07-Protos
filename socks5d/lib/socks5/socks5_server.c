//
// Created by Lucas Dell'Isola on 30/05/2022.
//

#include <malloc.h>
#include <string.h>
#include "socks5/socks5_server.h"
#include "utils/utils.h"
#include "utils/logger.h"



TcpSocket *InitSocks5Server(const char *port) {
    LogInfo("Starting SOCKS5 server...");
    TcpSocket * tcpSocket = InitTcpServer(port);
    LogInfo("SOCKS5 server up and running!");
    return tcpSocket;
}

Socks5Connection * WaitForNewSocks5Connections(TcpSocket *server) {
    LogInfo( "Waiting for SOCKS5 connections...");

    TcpSocket * tcpSocket = WaitForNewConnections(server);
    if (tcpSocket == null) {
        LogError(false,"Could not establish SOCKS5 connection");
        return null;
    }

    LogInfo("SOCKS5 connection established");
    LogInfo("Starting SOCKS5 handshake...");

    Socks5Connection * socks5Connection = Socks5ConnectionInit(tcpSocket);

    return socks5Connection;
}










