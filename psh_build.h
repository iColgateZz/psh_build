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

void psh_rebuild_unity(i32 argc, byte *argv[], byte *src[], usize src_count);
#define PSH_REBUILD_UNITY(argc, argv, ...)                                      \
        psh_rebuild_unity(argc, argv,  ((byte *[]){__FILE__, __VA_ARGS__}),     \
        (sizeof((byte *[]){__FILE__, __VA_ARGS__}) / sizeof(byte *)));

void psh_rebuild_unity_auto(i32 argc, byte *argv[], byte *source);
#define PSH_REBUILD_UNITY_AUTO(argc, argv)                   \
        psh_rebuild_unity_auto(argc, argv, __FILE__);

#define psh_shift(array, array_size) (PSH_ASSERT((array_size) > 0), (array_size)--, *(array)++)

#ifndef PSH_CC
    #define PSH_CC "gcc"
#endif

#ifndef PSH_CC_FLAGS 
    #define PSH_CC_FLAGS  "-Wall", "-Wextra", "-O2", "-Wno-initializer-overrides"
#endif

#ifndef PSH_CC_MORE_FLAGS
    #define PSH_CC_MORE_FLAGS ""
#endif

#ifndef PSH_CC_CMD
    #define PSH_CC_CMD(target, source1, ...) \
            PSH_CC, PSH_CC_FLAGS, PSH_CC_MORE_FLAGS, "-o", target, source1, __VA_ARGS__
#endif

#endif //PSH_BUILD_INCLUDE

#ifdef PSH_BUILD_IMPL

#define ARENA_IMPL
    #include "allocators/arena.h"
#define PSH_CORE_IMPL
    #include "psh_core/psh_core.h"

#include <sys/stat.h>

static inline psh_ternary psh__needs_rebuild(byte *executable, byte *src[], usize src_count);

void psh_rebuild_unity(i32 argc, byte *argv[], byte *src[], usize src_count) {
    byte *executable = psh_shift(argv, argc);
    byte *source = src[0];

    psh_ternary needs_rebuild = psh__needs_rebuild(executable, src, src_count);
    if (needs_rebuild == err) exit(EXIT_FAILURE);
    if (needs_rebuild == false) return;

    Psh_Cmd cmd = {0};
    psh_cmd_append(&cmd, PSH_CC_CMD(executable, source));
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

typedef struct {
    byte **items;
    usize count;
    usize capacity;
} Sources;

Sources psh__tokenize_deps(usize len, byte string[len]);
b32 psh__is_ws(byte c);
b32 psh__is_bs(byte c);
b32 psh__is_ws_or_bs(byte c);
b32 psh__is_alpha(byte c);
b32 psh__is_num(byte c);
b32 psh__is_path_symbol(byte c);
b32 psh__is_path(byte c);

void psh_rebuild_unity_auto(i32 argc, byte *argv[], byte *source) {
    Psh_Unix_Pipe pipe = {0};
    if (!psh_pipe_open(&pipe)) exit(EXIT_FAILURE);

    Psh_Cmd cmd = {0};
    psh_cmd_append(&cmd, "gcc", "-MM", source);
    if (!psh_cmd_run(&cmd, .fdout = pipe.write_fd)) exit(EXIT_FAILURE);
    psh_da_free(cmd);

    Psh_Fd_Reader reader = {.fd = pipe.read_fd};
    if (!psh_fd_read1(&reader)) exit(EXIT_FAILURE);

    Sources sources = psh__tokenize_deps(reader.store.count, reader.store.items);
    psh_rebuild_unity(argc, argv, sources.items, sources.count);

    psh_da_free(reader.store);
    psh_da_free(sources);
}

Sources psh__tokenize_deps(usize len, byte string[len]) {
    usize position = 0;
    Sources sources = {0};

    // skip until and over ':'
    while (string[position] != ':' && position < len) ++position;
    ++position;

    while (position < len) {
        while (psh__is_ws_or_bs(string[position]) && position < len) ++position;

        usize start = position;
        while (psh__is_path(string[position]) && position < len) ++position;

        psh_da_append(&sources, string + start);
        string[position] = 0;
        ++position;
        // printf("Dep: %s\n", string + start);
    }

    return sources;
}

b32 psh__is_ws(byte c) {
    return c == ' '  || c == '\n' ||
           c == '\t' || c == '\r' ;
}

b32 psh__is_bs(byte c) {
    return c == '\\';
}

b32 psh__is_ws_or_bs(byte c) {
    return psh__is_ws(c) || psh__is_bs(c);
}

b32 psh__is_alpha(byte c) {
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ;
}

b32 psh__is_num(byte c) {
    return '0' <= c && c <= '9';
}

b32 psh__is_path_symbol(byte c) {
    return c == '.' || c == '/' || 
           c == '_' || c == '-' ;
}

b32 psh__is_path(byte c) {
    return psh__is_path_symbol(c) ||
           psh__is_alpha(c)       ||
           psh__is_num(c)         ;
}

#endif //PSH_BUILD_IMPL
