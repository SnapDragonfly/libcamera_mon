#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

void log_message(LogLevel level, const char *format, ...);
bool is_print_log_level(LogLevel level);

#define LOG_DEBUG(fmt, ...) log_message(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) log_message(LOG_INFO, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...) log_message(LOG_WARNING, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_message(LOG_ERROR, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) log_message(LOG_FATAL, fmt, ##__VA_ARGS__)

#endif // LOG_H