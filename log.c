
#include "log.h"

LogLevel global_log_level           = LOG_INFO;

void log_message(LogLevel level, const char *format, ...) {
    if (level < global_log_level) return;

    switch (level) {
        case LOG_DEBUG: printf("[DEBUG] "); break;
        case LOG_INFO: printf("[INFO] "); break;
        case LOG_WARNING: printf("[WARNING] "); break;
        case LOG_ERROR: printf("[ERROR] "); break;
        case LOG_FATAL: printf("[FATAL] "); break;
    }

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

bool is_print_log_level(LogLevel level) {
    return (level >= global_log_level);
}
