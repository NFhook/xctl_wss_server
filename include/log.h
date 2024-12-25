#ifndef _LOG_H
#define _LOG_H

#include <glib.h>
#include <stdio.h>
#include <stdarg.h>

// log level define.
typedef enum {
    LOG_NONE = 0,
    LOG_FATAL = 1,
    LOG_ERR = 2,
    LOG_WARN = 3,
    LOG_INFO = 4,
    LOG_VERB = 5,
    LOG_HUGE = 6,
    LOG_DEBUG = 7,
    LOG_MAX = 8
} log_level;

// log color define.
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// log macro
#define LOG(level, fmt, ...) \
    logger_log(level, "[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_FATAL(...) LOG(LOG_FATAL, __VA_ARGS__)
#define LOG_ERROR(...) LOG(LOG_ERR, __VA_ARGS__)
#define LOG_WARN(...)  LOG(LOG_WARN, __VA_ARGS__)
#define LOG_INFO(...)  LOG(LOG_INFO, __VA_ARGS__)
#define LOG_VERB(...)  LOG(LOG_VERB, __VA_ARGS__)
#define LOG_DEBUG(...) LOG(LOG_DEBUG, __VA_ARGS__)

// function define.
void logger_init(const char* app_name, log_level level);
void logger_destroy(void);
void logger_set_level(log_level level);
void logger_set_log_file(const char* filename);
void logger_log(log_level level, const char* format, ...);

#endif
