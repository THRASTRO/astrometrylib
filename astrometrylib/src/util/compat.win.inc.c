#include "compat.win.inc.h"

// Return time in milliseconds?
uint64_t get_cpu_usage(bool aggregated) {

    FILETIME createTime;
    FILETIME exitTime;
    FILETIME kernelTime;
    FILETIME userTime;

    int rv = -1;
    if (aggregated) {
        rv = GetProcessTimes(GetCurrentProcess(),
            &createTime, &exitTime, &kernelTime, &userTime);
    }
    else {
        rv = GetThreadTimes(GetCurrentThread(),
            &createTime, &exitTime, &kernelTime, &userTime);
    }

    if (rv != -1)
    {
        ULONGLONG tcpu = userTime.dwHighDateTime;
        tcpu += kernelTime.dwHighDateTime;
        ULONGLONG usec = (tcpu << 32)
            + kernelTime.dwLowDateTime
            + userTime.dwLowDateTime;
        return (usec) / 10000;
    }

    return 0;
}

// Return time in milliseconds?
uint64_t get_wall_time()
{
    FILETIME twall;
    GetSystemTimeAsFileTime(&twall);
    ULONGLONG tcpu = twall.dwHighDateTime;
    ULONGLONG usec = (tcpu << 32) + twall.dwLowDateTime;
    return (usec - 11644473600000000ULL) / 10000;
}
