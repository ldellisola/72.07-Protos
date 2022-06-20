//
// Created by Lucas Dell'Isola on 20/06/2022.
//

#include <time.h>
#include "socks5/socks5_timeout.h"
#include "socks5/socks5_connection.h"
#include "utils/logger.h"
#include "utils/object_pool.h"

static time_t socksMaxTimeout = 100;


void CheckForTimeoutInSingleConnection(void * obj, void * data){
    Socks5Connection * connection = obj;
    fd_selector fdSelector = data;

    if (socksMaxTimeout <= 0)
        return;

    time_t currentTime = time(null);
    if ((time_t) -1 == currentTime)
    {
        ErrorWithReason("Cannot get current time");
        return;
    }

    if (currentTime - connection->LastConnectionOn >= socksMaxTimeout) {
        if (null == connection->RemoteAddressString) {
            LogInfo("Connection with client file descriptor %d and remote file descriptor %d timed out after %ld seconds",
                    connection->ClientTcpConnection->FileDescriptor,
                    connection->RemoteTcpConnection->FileDescriptor,
                    currentTime - connection->LastConnectionOn
            );
        }
        else{
            LogInfo("Connection to %s:%d timed out after %d seconds",
                    connection->RemoteAddressString,
                    connection->RemotePort,
                    currentTime - connection->LastConnectionOn
            );
        }
        DisposeSocks5Connection(connection, fdSelector);
    }


}

void CheckForTimeoutInSocks5Connections(fd_selector fdSelector) {
    ExecuteOnExistingElements(GetSocks5ConnectionPool(), CheckForTimeoutInSingleConnection, fdSelector);

}

void NotifySocks5ConnectionAccess(void *data) {
    Socks5Connection * connection = data;
    if (null == connection)
        return;

    connection->LastConnectionOn = time(null);

}

void SetSocks5ConnectionTimeout(time_t timeout) {
    socksMaxTimeout = timeout;
}

time_t GetSocks5ConnectionTimeout(){
    return socksMaxTimeout;
}
