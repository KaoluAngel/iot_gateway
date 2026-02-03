#ifndef LOGGER_H
#define LOGGER_H

/* 日志级别 */
typedef enum
{
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARN = 2,
    LOG_ERROR = 3,
    LOG_FATAL = 4
} log_level_t;
/* 编译时过滤：Release版本可定义 LOG_LEVEL_MIN 为 LOG_LEVEL_INFO */
#ifndef LOG_LEVEL_MIN
#define LOG_LEVEL_MIN LOG_LEVEL_DEBUG
#endif

/* 初始化日志 */
int logger_init(const char *log_file, log_level_t level);

/* 关闭日志 */
void logger_close(void);

/* 日志输出宏 */
void logger_log(log_level_t level, const char *format, ...);

#define LOG_DEBUG(fmt, ...) logger_log(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) logger_log(LOG_INFO, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) logger_log(LOG_WARN, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) logger_log(LOG_ERROR, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) logger_log(LOG_FATAL, fmt, ##__VA_ARGS__)

#endif
