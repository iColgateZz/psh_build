#include <stdio.h>
#include <string.h>

#define PSH_BUILD_IMPL
#define PSH_CORE_NO_PREFIX
#include "psh_build.h"

i32 main(i32 argc, byte *argv[]) {
    PSH_REBUILD(argc, argv);

    Arena perm_arena = arena_init(MB(1));
    if (!arena_check_init(perm_arena)) {
        printf("Something went wrong!\n");
        return -1;
    }

    byte *string = arena_push(&perm_arena, byte, 20);
    if (!string) {
        printf("Something went wrong 2!\n");
        return -1;
    }

    memcpy(string, "haaaii!!!", 11);

    Cmd cmd = {0};
    cmd_append(&cmd, "echo", string);

    if (!cmd_run(&cmd)) {
        printf("Something went wrong 3!\n");
        return -1;
    }

    return 0;
}
