#pragma once

#ifndef DEBUG_SC
#define UNUSED_IN_RELEASE __attribute__((unused))
#else
#define UNUSED_IN_RELEASE
#endif

namespace Util
{
    void setup(void);
    uint32_t FreeMem();
} // namespace Util

void dbgprintf(char const *str, ...);
