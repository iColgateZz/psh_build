#ifndef PSH_BUILD_INCLUDE
#define PSH_BUILD_INCLUDE

#include "arena.h"
#include "psh_core.h"



#endif

#ifdef PSH_BUILD_IMPL

#define ARENA_IMPL
    #include "arena.h"
#define PSH_CORE_IMPL
    #include "psh_core.h"

#endif