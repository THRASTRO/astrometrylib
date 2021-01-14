#include "compat.unix.inc.h"

int strcpy_s(char *dst, size_t size, const char *src)
{
    strncpy(dst, src, size);
    return 0;
}

// Return time in milliseconds
uint64_t get_cpu_usage(bool aggregated)
{
    return 0;
}

uint64_t get_wall_time()
{
    return 0;
}
