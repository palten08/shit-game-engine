#define _POSIX_C_SOURCE 199309L

#include <time.h>
#include <stdio.h>

#include "../include/logging.h"
#include "app.h"

static LogVerbosity g_log_level = LOG_VERBOSITY_DEBUG;
static LogMode g_log_mode = LOG_MODE_STDOUT;
static const char *g_log_path = NULL;

void log_engine_event(LogEvent event, const char *fmt, ...) {
    if ((int)event.type > (int)g_log_level) {
        return;
    }

    char message[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm *tm_info = localtime(&ts.tv_sec);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    int ms = ts.tv_nsec / 1000000;
    char full_timestamp[80];
    snprintf(full_timestamp, sizeof(full_timestamp), "%s.%03d", timestamp, ms);


    switch (g_log_mode) {
        case LOG_MODE_NONE:
            break;
        case LOG_MODE_FILE:
            break;
        case LOG_MODE_STDOUT:
            switch (event.type) {
                case MESSAGE_TYPE_ERROR:
                    fprintf(stderr, "[ERROR] [%s] %s:%d in %s: %s\n", full_timestamp, event.file, event.line, event.function, message);
                    break;
                case MESSAGE_TYPE_WARNING:
                    fprintf(stderr, "[WARNING] [%s] %s:%d in %s: %s\n", full_timestamp, event.file, event.line, event.function, message);
                    break;
                case MESSAGE_TYPE_INFO:
                    fprintf(stdout, "[INFO] [%s] %s:%d in %s: %s\n", full_timestamp, event.file, event.line, event.function, message);
                    break;
                case MESSAGE_TYPE_DEBUG:
                    fprintf(stdout, "[DEBUG] [%s] %s:%d in %s: %s\n", full_timestamp, event.file, event.line, event.function, message);
                    break;
            }
            break;
        }
}

void initialize_engine_logging(LogVerbosity verbosity, LogMode mode, const char *log_path) {
    g_log_level = verbosity;
    g_log_mode = mode;
    g_log_path = log_path;
}