#pragma once
#include <stdio.h>

#define YEL_RESET "\033[m"
#define YEL_RESET_STYLE "\033[22m\033[24m\033[29m"
#define YEL_RESET_COLOR "\033[m"

#define YEL_BLUE "\033[34m"
#define YEL_YELLOW "\033[33m"
#define YEL_RED "\033[31m"

#define YEL_BOLD "\033[1m"
#define YEL_UNDERLINE "\033[4m"
#define YEL_STRIKE "\033[9m"

#define CUSTOM_LOG(...)                                                        \
    do {                                                                       \
        printf(__VA_ARGS__);                                                   \
        printf("%s", YEL_RESET);                                               \
    } while (0)

#define YEL_INFO(...) CUSTOM_LOG(YEL_BLUE __VA_ARGS__)
#define YEL_WARN(...) CUSTOM_LOG(YEL_YELLOW __VA_ARGS__)
#define YEL_CRIT(...) CUSTOM_LOG(YEL_RED __VA_ARGS__)
#define YEL_ECHO(...) CUSTOM_LOG(__VA_ARGS__)
