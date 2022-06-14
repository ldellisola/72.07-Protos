//
// Created by Lucas Dell'Isola on 30/05/2022.
//

#include <stdbool.h>

#ifndef SERVER_LOGGER_H
#define SERVER_LOGGER_H

#endif //SERVER_LOGGER_H

typedef enum {
    LOG_INFO,
    LOG_ERROR
} LOG_TYPE;

void SetLogLevel(LOG_TYPE minimumType);

void LogInfo(const char *message, ...);

void LogError(bool showInnerError, const char *message, ...);



