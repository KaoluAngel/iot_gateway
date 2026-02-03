#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include "log.h"

static FILE *log_fp = NULL;
static log_level_t log_level = LOG_INFO;
static pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;

static const char *level_names[] = {
    "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

int logger_init(const char *log_file, log_level_t level) {
    pthread_mutex_lock(&log_lock);
    
    log_fp = fopen(log_file, "a");
    if (!log_fp) {
        fprintf(stderr, "Failed to open log file: %s\n", log_file);
        pthread_mutex_unlock(&log_lock);
        return -1;
    }
    
    log_level = level;
    setvbuf(log_fp, NULL, _IOLBF, 256);  /* 行缓冲 */
    
    pthread_mutex_unlock(&log_lock);
    return 0;
}

void logger_close(void) {
    pthread_mutex_lock(&log_lock);
    if (log_fp) {
        fclose(log_fp);
        log_fp = NULL;
    }
    pthread_mutex_unlock(&log_lock);
}

void logger_log(log_level_t level, const char *format, ...) {
    if (level < log_level) {
        return;  /* 日志级别过低，忽略 */
    }
    if (level < 0 || level >= (int)(sizeof(level_names)/sizeof(level_names[0]))) {
        level = 0; /* 兜底为 DEBUG */
    }

    /* 获取带毫秒的时间戳（线程安全） */
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    time_t sec = ts.tv_sec;
    struct tm tm_info;
    localtime_r(&sec, &tm_info);
    char time_part[32];
    strftime(time_part, sizeof(time_part), "%Y-%m-%d %H:%M:%S", &tm_info);
    int ms = (int)(ts.tv_nsec / 1000000);
    char timestamp[40];
    snprintf(timestamp, sizeof(timestamp), "%s.%03d", time_part, ms);

    /* 格式化消息到缓冲区，避免重复使用 va_list，并保证原子输出 */
    char msgbuf[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(msgbuf, sizeof(msgbuf), format, args);
    va_end(args);

    char outbuf[1280];
    int outlen = snprintf(outbuf, sizeof(outbuf), "[%s] [%s] %s\n",
                          timestamp, level_names[level], msgbuf);

    pthread_mutex_lock(&log_lock);
    fwrite(outbuf, 1, (size_t)outlen, stderr);
    if (log_fp) {
        fwrite(outbuf, 1, (size_t)outlen, log_fp);
    }
    pthread_mutex_unlock(&log_lock);
}
