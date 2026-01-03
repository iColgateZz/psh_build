#ifndef PSH_BUILD_INCLUDE
#define PSH_BUILD_INCLUDE

#include "arena.h"
#include "psh_core.h"

#ifndef DEFINITIONS_INCLUDE
#define DEFINITIONS_INCLUDE

#include <stdint.h>
#include <stddef.h>

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef float       f32;
typedef double      f64;

typedef char        byte;
typedef int32_t     b32;

typedef uintptr_t   uptr;
typedef ptrdiff_t   isize;
typedef size_t      usize;

#define true 1
#define false 0

#endif

void psh_rebuild_();
#define psh_rebuild(argc, argv)

#endif

#ifdef PSH_BUILD_IMPL

#define ARENA_IMPL
    #include "arena.h"
#define PSH_CORE_IMPL
    #include "psh_core.h"

#endif