/**
 * @file gs_debug.hpp
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Contains debug-related macros and function-like macros.
 * @version 0.1
 * @date 2021.07.26
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef MEB_DEBUG_HPP
#define MEB_DEBUG_HPP

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#ifndef MEB_COLORS
#define MEB_COLORS
#define RESET_ALL "\x1b[0m"
#define RED_FG "\x1b[91m"
#define GREEN_FG "\x1b[92m"
#define YELLOW_FG "\x1b[33m"
#define BLUE_FG "\x1b[94m"
#define MAGENTA_FG "\x1b[95m"
#define CYAN_FG "\x1b[96m"
#define RED_BG "\x1b[101m"
#define GREEN_BG "\x1b[102m"
#define YELLOW_BG "\x1b[43m"
#define BLUE_BG "\x1b[104m"
#define MAGENTA_BG "\x1b[105m"
#define CYAN_BG "\x1b[106m"
#define MEB_CLR "\x1b[0m"
#endif // MEB_COLORS

#ifndef MEB_CODES
#define MEB_CODES
#define FATAL "\033[1m\x1b[107m\x1b[31m(FATAL) "
#endif // MEB_CODES

#ifndef dbprintlf
#define dbprintlf(format, ...)                                                                         \
    fprintf(stderr, "[%s:%d | %s] " format MEB_CLR "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    fflush(stderr);
#endif // dbprintlf

#ifndef dbprintf
#define dbprintf(format, ...)                                                                     \
    fprintf(stderr, "[%s:%d | %s] " format MEB_CLR, __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    fflush(stderr);
#endif // dbprintf

#ifndef erprintlf
#define erprintlf(error)                                                                                                     \
    fprintf(stderr, "[%s:%d | %s] " RED_FG ">>> %d: %s" MEB_CLR "\n", __FILE__, __LINE__, __func__, error, strerror(error)); \
    fflush(stderr);
#endif // erprintlf

#ifndef errprintlf
#define errprintlf(errmsg)                                                                                                        \
    {                                                                                                                             \
        fprintf(stderr, "[%s:%d | %s] " RED_FG ">>> %s: %s" MEB_CLR "\n", __FILE__, __LINE__, __func__, errmsg, strerror(errno)); \
        fflush(stdout);                                                                                                           \
    }
#endif

static char *get_time_now()
{
    static __thread char buf[128];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buf, sizeof(buf), GREEN_FG "[" YELLOW_FG "%02d:%02d:%02d" GREEN_FG "] " MEB_CLR,
             tm.tm_hour, tm.tm_min, tm.tm_sec);
    return buf;
}

static char *get_datetime_now()
{
    static __thread char buf[128];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buf, sizeof(buf), GREEN_FG "[" YELLOW_FG "%04d-%02d-%-2d %02d:%02d:%02d" GREEN_FG "] " MEB_CLR,
             tm.tm_year + 1900, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return buf;
}

static char *get_time_now_raw()
{
    static __thread char buf[128];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buf, sizeof(buf), "%02d%02d%02d",
             tm.tm_hour, tm.tm_min, tm.tm_sec);
    return buf;
}

static char *get_datetime_now_raw()
{
    static __thread char buf[128];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buf, sizeof(buf), "%04d%02d%2d_%02d%02d%02d",
             tm.tm_year + 1900, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return buf;
}

#ifndef tprintf
#define tprintf(str, ...)                                        \
    {                                                            \
        printf("%s" str MEB_CLR, get_time_now(), ##__VA_ARGS__); \
        fflush(stdout);                                          \
    }
#endif

#ifndef tprintlf
#define tprintlf(str, ...)                                            \
    {                                                                 \
        printf("%s" str MEB_CLR "\n", get_time_now(), ##__VA_ARGS__); \
        fflush(stdout);                                               \
    }
#endif

#ifndef bprintf
#define bprintf(str, ...)                   \
    {                                       \
        printf(str MEB_CLR, ##__VA_ARGS__); \
        fflush(stdout);                     \
    }
#endif

#ifndef bprintlf
#define bprintlf(str, ...)                       \
    {                                            \
        printf(str MEB_CLR "\n", ##__VA_ARGS__); \
        fflush(stdout);                          \
    }
#endif

#endif // MEB_DEBUG_HPP