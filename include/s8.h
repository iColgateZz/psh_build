#ifndef STRING_INCLUDE
#define STRING_INCLUDE

#include <stddef.h>

typedef struct {
    char *s;
    size_t len;
} s8;

#define countof(x)              (size_t)(sizeof(x) / sizeof(*(x)))
#define lenof(s)                countof(s) - 1

#define s8(...)                       s8_(__VA_ARGS__, s8_from_heap, s8_read_only)(__VA_ARGS__)
#define s8_(a, b, c, ...)             c
#define s8_read_only(str)             (s8){.s = str, .len = lenof(str)}
#define s8_from_heap(str, str_len)    (s8){.s = str, .len = str_len}

#endif