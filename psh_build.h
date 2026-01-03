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

typedef i32 psh_ternary;
#define err -1

void psh_rebuild(i32 argc, byte *argv[], byte *source, ...);
#define PSH_REBUILD(argc, argv)           psh_rebuild(argc, argv, __FILE__, NULL);
#define PSH_REBUILD_MANY(argc, argv, ...) psh_rebuild(argc, argv, __FILE__, __VA_ARGS__, NULL);

#define psh_shift(array, array_size) (PSH_ASSERT((array_size) > 0), (array_size)--, *(array)++)

#ifndef psh_cc
    #define psh_cc(cmd) psh_cmd_append(cmd, "gcc")
#endif

#ifndef psh_cc_out
    #define psh_cc_out(cmd, output) psh_cmd_append(cmd, "-o", output)
#endif

#ifndef psh_cc_in
    #define psh_cc_in(cmd, ...) psh_cmd_append(cmd, __VA_ARGS__)
#endif

#ifndef psh_cc_flags
    #define psh_cc_flags(cmd) psh_cmd_append(cmd, "-Wall", "-Wextra")
#endif

#endif

#ifdef PSH_BUILD_IMPL

#define ARENA_IMPL
    #include "arena.h"
#define PSH_CORE_IMPL
    #include "psh_core.h"

static inline psh_ternary psh__needs_rebuild(byte *executable, byte *src[], usize srcnum);

void psh_rebuild(i32 argc, byte *argv[], byte *source, ...) {
    byte *executable = psh_shift(argv, argc);

    struct {
        byte **items;
        usize count;
        usize capacity;
    } sources = {0};
    psh_da_append(&sources, source);

    va_list args;
    va_start(args, source);
    while (true) {
        byte *path = va_arg(args, byte *);
        if (path == NULL) break;
        psh_da_append(&sources, path);
    }
    va_end(args);

    psh_ternary needs_rebuild = psh__needs_rebuild(executable, sources.items, sources.count);
    if (needs_rebuild == err) exit(EXIT_FAILURE);
    if (needs_rebuild == false) {
        psh_da_free(sources);
        return;
    }

    Psh_Cmd cmd = {0};
    psh_cc(&cmd);
    psh_cc_out(&cmd, executable);
    psh_cc_in(&cmd, source);
    psh_cc_flags(&cmd);
    psh_cmd_append(&cmd, "-Iarena_allocator", "-Ipsh_core");
    if (!psh_cmd_run(&cmd)) exit(EXIT_FAILURE);

    psh_cmd_append(&cmd, executable);
    psh_da_append_many(&cmd, argv, argc);
    if (!psh_cmd_run(&cmd)) exit(EXIT_FAILURE);

    exit(EXIT_SUCCESS);
}

static inline
psh_ternary psh__needs_rebuild(byte *executable, byte *src[], usize srcnum) {
    PSH_UNUSED(executable);
    PSH_UNUSED(src);
    PSH_UNUSED(srcnum);
    return true;
}

#endif