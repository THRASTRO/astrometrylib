#include "compat.h"

uint32_t ENDIAN_DETECTOR = 0x01020304;

#ifdef _WIN32
# include "compat.win.inc.c"
#else
# include "compat.unix.inc.c"
#endif
