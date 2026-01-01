#include "types.h"
#include <stdio.h>
#include <string.h>

#define ARENA_IMPL
    #include "arena.h"
#define PSH_CORE_NO_PREFIX
#define PSH_CORE_IMPL
    #include "psh_core.h"

i32 main(void) {
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
