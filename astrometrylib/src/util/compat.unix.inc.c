#include "compat.unix.inc.h"

#define __USE_GNU
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

int strcpy_s(char *dst, size_t size, const char *src)
{
    strncpy(dst, src, size);
    return 0;
}

// Return time in milliseconds?
uint64_t get_cpu_usage(bool aggregated)
{
    struct rusage r;
    uint64_t sofar;
    if (getrusage(aggregated ? RUSAGE_SELF : RUSAGE_THREAD, &r)) {
        SYSERROR("Failed to get resource usage");
        return -1.0;
    }
    sofar = 1e+3 * (r.ru_utime.tv_sec + r.ru_stime.tv_sec) +
        1e-3 * (r.ru_utime.tv_usec + r.ru_stime.tv_usec);
    return sofar;
}

// Return time in milliseconds?
uint64_t get_wall_time()
{
    struct timeval tv;
    int rv = gettimeofday(&tv, NULL);
    return tv.tv_sec * 1e+3 + tv.tv_usec * 1e-3;
}
