#ifndef PSH_BUILD_INCLUDE
#define PSH_BUILD_INCLUDE

#include "allocators/arena.h"
#include "psh_core/psh_core.h"

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

#ifndef PSH_CC
    #define PSH_CC "gcc"
#endif
#ifndef psh_cc
    #define psh_cc(cmd) psh_cmd_append(cmd, PSH_CC)
#endif

#ifndef PSH_CC_OUT
    #define psh_cc_out(cmd, output) psh_cmd_append(cmd, "-o", output)
#else
    #define psh_cc_out(cmd, output) psh_cmd_append(cmd, "-o", PSH_CC_OUT)
#endif

#ifndef psh_cc_in
    #define psh_cc_in(cmd, ...) psh_cmd_append(cmd, __VA_ARGS__)
#endif

#ifndef PSH_CC_FLAGS 
    #define PSH_CC_FLAGS  "-Wall", "-Wextra", "-O2"
#endif
#ifndef psh_cc_flags
    #define psh_cc_flags(cmd) psh_cmd_append(cmd, PSH_CC_FLAGS)
#endif

#ifdef PSH_CC_MORE
    #define psh_cc_more(cmd) psh_cmd_append(cmd, PSH_CC_MORE)
#else
    #define psh_cc_more(cmd) PSH_UNUSED(cmd)
#endif

#endif //PSH_BUILD_INCLUDE

#ifdef PSH_BUILD_IMPL

#define ARENA_IMPL
    #include "allocators/arena.h"
#define PSH_CORE_IMPL
    #include "psh_core/psh_core.h"

#include <sys/stat.h>

static inline psh_ternary psh__needs_rebuild(byte *executable, byte *src[], usize src_count);

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
    // even if there are multiple sources, the prog assumes a unity build
    psh_cc_in(&cmd, source);
    psh_cc_flags(&cmd);
    psh_cc_more(&cmd);
    if (!psh_cmd_run(&cmd)) exit(EXIT_FAILURE);

    psh_cmd_append(&cmd, executable);
    psh_da_append_many(&cmd, argv, argc);
    if (!psh_cmd_run(&cmd)) exit(EXIT_FAILURE);

    exit(EXIT_SUCCESS);
}

static inline
psh_ternary psh__needs_rebuild(byte *executable, byte *src[], usize src_count) {
    struct stat statbuf = {0};
    if (stat(executable, &statbuf) < 0) {
        // Executable does not exist
        if (errno == ENOENT) return true;

        psh_logger(PSH_ERROR, "could not get info about executable %s: %s", executable, strerror(errno));
        return err;
    }
    u32 exec_mod_time = statbuf.st_mtime;

    for (usize i = 0; i < src_count; ++i) {
        byte *source = src[i];
        if (stat(source, &statbuf) < 0) {
            psh_logger(PSH_ERROR, "could not get info about source %s: %s", source, strerror(errno));
            return err;
        }

        u32 source_mod_time = statbuf.st_mtime;
        if (source_mod_time > exec_mod_time) return true;
    }

    return false;
}

#endif //PSH_BUILD_IMPL
