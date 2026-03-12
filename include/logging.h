#pragma once

#include <time.h>
#include <stdarg.h>

#include "types.h"


#define LOG_ERROR(fmt, ...) log_engine_event((LogEvent){ \
    .file = __FILE__, \
    .function = __func__, \
    .line = __LINE__, \
    .type = MESSAGE_TYPE_ERROR \
}, fmt, ##__VA_ARGS__)

#define LOG_WARNING(fmt, ...) log_engine_event((LogEvent){ \
    .file = __FILE__, \
    .function = __func__, \
    .line = __LINE__, \
    .type = MESSAGE_TYPE_WARNING \
}, fmt, ##__VA_ARGS__)

#define LOG_INFO(fmt, ...) log_engine_event((LogEvent){ \
    .file = __FILE__, \
    .function = __func__, \
    .line = __LINE__, \
    .type = MESSAGE_TYPE_INFO \
}, fmt, ##__VA_ARGS__)

#define LOG_DEBUG(fmt, ...) log_engine_event((LogEvent){ \
    .file = __FILE__, \
    .function = __func__, \
    .line = __LINE__, \
    .type = MESSAGE_TYPE_DEBUG \
}, fmt,##__VA_ARGS__)

typedef enum {
    MESSAGE_TYPE_ERROR = 0,
    MESSAGE_TYPE_WARNING,
    MESSAGE_TYPE_INFO,
    MESSAGE_TYPE_DEBUG
} MessageType;

typedef enum {
    LOG_MODE_NONE = 0,
    LOG_MODE_STDOUT,
    LOG_MODE_FILE
} LogMode;

typedef enum {
    LOG_VERBOSITY_ERROR = 0,
    LOG_VERBOSITY_WARNING,
    LOG_VERBOSITY_INFO,
    LOG_VERBOSITY_DEBUG
} LogVerbosity;

typedef struct {
    const char *file;
    const char *function;
    int line;
    const char *message;
    const char *timestamp;
    MessageType type;
} LogEvent;

void log_engine_event(LogEvent event, const char *fmt, ...);
void initialize_engine_logging(LogVerbosity verbosity, LogMode mode, const char *log_path);