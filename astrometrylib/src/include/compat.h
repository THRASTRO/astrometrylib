#ifndef KDTREE_COMPAT_H
#define KDTREE_COMPAT_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <errno.h>
#include <math.h>

// upper bound of dimquads value
#define DQMAX 5
// upper bound of dimcodes value
#define DCMAX 6

// simple min/max implementations
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// Static value for endian detection
extern uint32_t ENDIAN_DETECTOR;

// Return time in milliseconds
uint64_t get_cpu_usage(bool);
uint64_t get_wall_time();

#ifdef FOO
#define SYSERROR printf
#define ANERROR printf

#define debug printf
#define logmsg printf
#define logerr printf
#define logverb printf
#define logdebug printf
#define loglevel(lvl, fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define SYSERROR printf
#define ANERROR printf
#define debug
#define logmsg
#define logerr
#define logverb
#define logdebug 
#define loglevel(lvl, fmt, ...)
#endif
#define debug2(...)
// #define qfits_error printf
#define qdebug

// MSVC compatibility
#ifdef _MSC_VER
#define strdup _strdup
#define fseeko _fseeki64
#define ftello _ftelli64
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#define streq(a,b) (strcmp((a),(b))==0)
#define strcaseeq(a,b) (_stricmp((a),(b))==0)
#endif

void QSORT_R(void* base, size_t nmembers, size_t member_size,
    void* token, int (*compar)(void*, const void*, const void*));

#define QSORT_COMPARISON_FUNCTION(func, thunk, v1, v2) func(thunk, v1, v2)

#endif