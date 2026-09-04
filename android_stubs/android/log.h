#ifndef _ANDROID_LOG_H
#define _ANDROID_LOG_H

#include <stdarg.h>
#include <stdio.h>

typedef enum android_LogPriority {
    ANDROID_LOG_UNKNOWN = 0,
    ANDROID_LOG_DEFAULT,
    ANDROID_LOG_VERBOSE,
    ANDROID_LOG_DEBUG,
    ANDROID_LOG_INFO,
    ANDROID_LOG_WARN,
    ANDROID_LOG_ERROR,
    ANDROID_LOG_FATAL,
    ANDROID_LOG_SILENT,
} android_LogPriority;

#ifdef __cplusplus
extern "C" {
#endif

// Simple implementation for non-Android builds
static inline int __android_log_print(int prio, const char *tag, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
    return 0;
}

static inline int __android_log_vprint(int prio, const char *tag, const char *fmt, va_list ap) {
    return vprintf(fmt, ap);
}

static inline int __android_log_write(int prio, const char *tag, const char *text) {
    printf("%s\n", text);
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif // _ANDROID_LOG_H
