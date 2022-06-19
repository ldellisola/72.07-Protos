//
// Created by Lucas Dell'Isola on 30/05/2022.
//


#ifndef SERVER_LOGGER_H
#define SERVER_LOGGER_H

#include <stdbool.h>

typedef enum {
    /**
     * Used for debug purposes.
     */
    LOG_DEBUG,
    /**
     * Uses for normal, user facing messages
     */
    LOG_INFO,
    /**
     * Used for errors that are recoverable
     */
    LOG_WARNING,
    /**
     * Used for errors that will stop and dispose the current connection.
     */
    LOG_ERROR,
    /**
     * Used for errors that will stop and crash the server
     */
    LOG_FATAL
} LOG_TYPE;

void SetLogLevel(LOG_TYPE minimumType);

void Log(LOG_TYPE type,const char * file, int line,bool hasInnerError, const char * format, ...);



/**
 * It prints a message without parameters as a DEBUG log
 * @param message message
 */
#define Debug(message) Log(LOG_DEBUG,__FILE__,__LINE__,false,message,null)

/**
 * It prints a formatted message with parameters as a DEBUG log
 * @param message message
 */
#define LogDebug(message,...) Log(LOG_DEBUG,__FILE__,__LINE__,false,message,__VA_ARGS__)

/**
 * It prints a message without parameters as an INFO log
 * @param message message
 */
#define Info(message) Log(LOG_INFO,__FILE__,__LINE__,false,message,null)

/**
 * It prints a formatted message with parameters as an INFO log
 * @param message message
 */
#define LogInfo(message,...) Log(LOG_INFO,__FILE__,__LINE__,false,message,__VA_ARGS__)


/**
 * It prints a message without parameters as a WARNING log
 * @param message message
 */
#define Warning(message) Log(LOG_WARNING,__FILE__,__LINE__,false,message,null)

/**
 * It prints a message without parameters as a WARNING log and it shows the current errno message
 * @param message message
 */
#define WarningWithReason(message) Log(LOG_WARNING,__FILE__,__LINE__,true,message,null)

/**
 * It prints a formatted message with parameters as a WARNING log
 * @param message message
 */
#define LogWarning(message,...) Log(LOG_WARNING,__FILE__,__LINE__,false,message,__VA_ARGS__)

/**
 * It prints a formatted message with parameters as a WARNING log and it shows the current errno message
 * @param message message
 */
#define LogWarningWithReason(message,...) Log(LOG_WARNING,__FILE__,__LINE__,true,message,__VA_ARGS__)

/**
 * It prints a message without parameters as an ERROR log
 * @param message message
 */
#define Error(message) Log(LOG_ERROR,__FILE__,__LINE__,false,message,null)

/**
 * It prints a message without parameters as an ERROR log and it shows the current errno message
 * @param message message
 */
#define ErrorWithReason( format) Log(LOG_ERROR,__FILE__,__LINE__,true,format,null)

/**
 * It prints a formatted message with parameters as an ERROR log
 * @param message message
 */
#define LogError(format,...) Log(LOG_ERROR,__FILE__,__LINE__,false,format,__VA_ARGS__)

/**
 * It prints a formatted message with parameters as an ERROR log and it shows the current errno message
 * @param message message
 */
#define LogErrorWithReason(format,...) Log(LOG_ERROR,__FILE__,__LINE__,true,format,__VA_ARGS__)

/**
 * It prints a message without parameters as a FATAL log
 * @param message message
 */
#define Fatal(message) Log(LOG_FATAL,__FILE__,__LINE__,false,message,null)

/**
 * It prints a message without parameters as a FATAL log and it shows the current errno message
 * @param message message
 */
#define FatalWithReason(message) Log(LOG_FATAL,__FILE__,__LINE__,true,message,null)

/**
 * It prints a formatted message with parameters as a FATAL log
 * @param message message
 */
#define LogFatal(format, ...) Log(LOG_FATAL,__FILE__,__LINE__,false,format,__VA_ARGS__)

/**
 * It prints a formatted message with parameters as a FATAL log and it shows the current errno message
 * @param message message
 */
#define LogFatalWithReason(format, ...) Log(LOG_FATAL,__FILE__,__LINE__,true,format,__VA_ARGS__)


#endif //SERVER_LOGGER_H


