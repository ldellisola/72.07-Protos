//
// Created by Lucas Dell'Isola on 30/05/2022.
//

#include <stdio.h>
#include <stdarg.h>
#include "utils/logger.h"
#include "utils/utils.h"
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

LOG_TYPE LogLevel = LOG_ERROR;


void SetLogLevel(LOG_TYPE minimumType) {
    LogLevel = minimumType;
}

void Log(LOG_TYPE type, const char *file, int line, bool hasInnerError, const char *format, ...) {

    if (type < LogLevel)
        return;

    va_list arg;
    va_start (arg, format);

    FILE * output;
    const char *  title;

    switch (type) {
        case LOG_DEBUG:
            output = stdout;
            title = "DEBUG";
            fprintf(output, "%s on File %s, line %d:\n\t",title,file,line);
            break;
        case LOG_INFO:
            output = stdout;
            title = "INFO";
            fprintf(output, "%s: ",title);
            break;
        case LOG_WARNING:
            output = stderr;
            title = "WARNING";
            fprintf(output, "%s on File %s, line %d:\n\t",title,file,line);
            break;
        case LOG_ERROR:
            output = stderr;
            title = "ERROR";
            fprintf(output, "%s on File %s, line %d:\n\t",title,file,line);
            break;
        case LOG_FATAL:
            output = stderr;
            title = "FATAL";
            fprintf(output, "%s on File %s, line %d:\n\t",title,file,line);
            break;
        default:
            output = stderr;
            title = "?????";
            fprintf(output, "%s on File %s, line %d:\n\t",title,file,line);
            break;
    }

    vfprintf(output, format, arg);
    fprintf(output, "\n");
    if (hasInnerError)
        fprintf(output, "\tReason: %s\n", strerror(errno));

    va_end (arg);

    if (LOG_FATAL == type)
        exit(1);
}

void SetLogLevelFromEnvironment() {
    const char * level = getenv("SOCKS5D_LOG_LEVEL");
    if (null == level)
    {
        SetLogLevel(LOG_INFO);
        Info("SOCKS5D_LOG_LEVEL environment variable not set. Using default level INFO");
        return;
    }

    if (0 == strcmp("INFO",level))
        SetLogLevel(LOG_INFO);
    else if (0 == strcmp("DEBUG",level))
        SetLogLevel(LOG_DEBUG);
    else if (0 == strcmp("WARN",level))
        SetLogLevel(LOG_WARNING);
    else if (0 == strcmp("ERROR",level))
        SetLogLevel(LOG_ERROR);
    else if (0 == strcmp("FATAL",level))
        SetLogLevel(LOG_FATAL);
    else{
        SetLogLevel(LOG_INFO);
        Info("SOCKS5D_LOG_LEVEL environment variable not set. Using default level INFO");
    }
}


