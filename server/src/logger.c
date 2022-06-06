//
// Created by Lucas Dell'Isola on 30/05/2022.
//

#include <stdio.h>
#include <stdarg.h>
#include "../headers/logger.h"
#include <errno.h>
#include <stdbool.h>
#include <string.h>

LOG_TYPE LogLevel = LOG_ERROR;

void LogInfo(const char *message, ...) {
    if (LogLevel != LOG_INFO)
        return;

    va_list arg;
    va_start (arg, message);

    fprintf(stdout,"INFO: ");
    vfprintf (stdout, message, arg);
    fprintf(stdout,"\n");

    va_end (arg);
}

void LogError(bool showInnerError, const char *message, ...) {
    va_list arg;
    va_start (arg, message);

            fprintf(stderr, "ERROR: ");
            vfprintf (stderr, message, arg);
            fprintf(stderr,"\n");
            if (showInnerError)
                fprintf(stderr,"\tReason: %s\n",strerror(errno));

    va_end (arg);
}

void SetLogLevel(LOG_TYPE minimumType) {
    LogLevel = minimumType;
}
