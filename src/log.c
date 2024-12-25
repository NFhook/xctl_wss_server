#include "log.h"
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

static struct {
    log_level level;          // 当前日志级别
    char *app_name;          // 应用名称
    FILE *log_file;          // 日志文件句柄
    GMutex mutex;            // 互斥锁
    gboolean use_colors;     // 是否使用颜色
    gboolean initialized;     // 是否已初始化
} logger = {
    .level = LOG_INFO,
    .app_name = NULL,
    .log_file = NULL,
    .initialized = FALSE,
    .use_colors = TRUE
};

// 日志级别对应的字符串
static const char *log_level_strings[] = {
    "NONE",
    "FATAL",
    "ERR",
    "WARN",
    "INFO",
    "VERB",
    "HUGE",
    "DEBUG"
};

// 日志级别对应的颜色
static const char *log_colors[] = {
    "",                     // NONE
    ANSI_COLOR_RED,        // FATAL
    ANSI_COLOR_RED,        // ERROR
    ANSI_COLOR_YELLOW,     // WARN
    ANSI_COLOR_GREEN,      // INFO
    ANSI_COLOR_CYAN,       // VERB
    ANSI_COLOR_BLUE,       // HUGE
    ANSI_COLOR_MAGENTA     // DEBUG
};

void logger_init(const char* app_name, log_level level) {
    if(logger.initialized)
        return;

    g_mutex_init(&logger.mutex);
    logger.app_name = g_strdup(app_name);
    logger.level = level;
    logger.initialized = TRUE;

    // 检查是否在终端环境
    logger.use_colors = isatty(STDOUT_FILENO);
}

void logger_destroy(void) {
    if(!logger.initialized)
        return;

    g_mutex_clear(&logger.mutex);
    g_free(logger.app_name);

    if(logger.log_file)
        fclose(logger.log_file);

    logger.initialized = FALSE;
}

void logger_set_level(log_level level) {
    if(!logger.initialized)
        return;
    logger.level = level;
}

void logger_set_log_file(const char* filename) {
    if(!logger.initialized)
        return;

    g_mutex_lock(&logger.mutex);

    if(logger.log_file)
        fclose(logger.log_file);

    logger.log_file = fopen(filename, "a");
    if(!logger.log_file) {
        fprintf(stderr, "Error opening log file: %s\n", filename);
    }

    g_mutex_unlock(&logger.mutex);
}

void logger_log(log_level level, const char* format, ...) {
    if(!logger.initialized || level > logger.level)
        return;

    g_mutex_lock(&logger.mutex);

    // 获取时间戳
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t t = tv.tv_sec;
    struct tm *tm = localtime(&t);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm);

    // 准备日志前缀
    char prefix[512];
    g_snprintf(prefix, sizeof(prefix), "[%s] %s.%03d %s ",
        logger.app_name,
        timestamp,
        (int)(tv.tv_usec/1000),
        log_level_strings[level]
    );

    // 处理可变参数
    va_list args;
    va_start(args, format);

    // 输出到控制台
    if(logger.use_colors) {
        printf("%s%s%s", log_colors[level], prefix, ANSI_COLOR_RESET);
        vprintf(format, args);
        printf("\n");
    } else {
        printf("%s", prefix);
        vprintf(format, args);
        printf("\n");
    }

    // 输出到文件
    if(logger.log_file) {
        fprintf(logger.log_file, "%s", prefix);
        vfprintf(logger.log_file, format, args);
        fprintf(logger.log_file, "\n");
        fflush(logger.log_file);
    }

    va_end(args);
    g_mutex_unlock(&logger.mutex);
}
