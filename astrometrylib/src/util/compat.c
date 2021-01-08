#include "compat.h"

uint32_t ENDIAN_DETECTOR = 0x01020304;

#ifdef _WIN32

#include <Windows.h>

uint64_t get_cpu_usage() {

    FILETIME createTime;
    FILETIME exitTime;
    FILETIME kernelTime;
    FILETIME userTime;

    if (GetThreadTimes(GetCurrentThread(),
        &createTime, &exitTime, &kernelTime, &userTime) != -1)
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

uint64_t get_wall_time()
{
    FILETIME twall;
    GetSystemTimeAsFileTime(&twall);
    ULONGLONG tcpu = twall.dwHighDateTime;
    ULONGLONG usec = (tcpu << 32) + twall.dwLowDateTime;
    return (usec - 11644473600000000ULL) / 10000;
}

#else

// To be implemented

#endif
