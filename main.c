#include <stdio.h>
#include <string.h>

#define PSH_BUILD_IMPL
#define PSH_CORE_NO_PREFIX
    #include "psh_build.h"

i32 main(i32 argc, byte *argv[]) {
    PSH_REBUILD_UNITY_AUTO(argc, argv);

    Arena perm_arena = arena_init(MB(1));

    byte *string = arena_push(&perm_arena, byte, 20);
    if (!string) {
        printf("Something went wrong!\n");
        return -1;
    }

    byte str[] = "haaaii!!!";
    memcpy(string, str, sizeof str);

    Cmd cmd = {0};
    cmd_append(&cmd, "echo", string);

    if (!cmd_run(&cmd)) {
        printf("Something went wrong 2!\n");
        return -1;
    }

    return 0;
}
