/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "Utils.h"

#include <cstdio>
#include <cstring>
#include <chrono>

uint64_t getCurrentTime()
{
    using namespace std::chrono;
    auto now = system_clock::now();
    auto us = duration_cast<microseconds>(now.time_since_epoch()).count();
    return static_cast<uint64_t>(us);
}

const char* formatNum(int num, char delimiter)
{
    static char Out[20];

    char buf[sizeof(Out)];
    snprintf(buf, sizeof(buf), "%d", num);

    const uint32_t len = strlen(buf);
    const uint32_t skip = len % 3;
    uint32_t idx = skip;
    if (skip)
    {
        memcpy(Out, buf, skip);
        if (len > 3)
        {
            Out[idx++] = delimiter;
        }
    }

    for (uint32_t pos = 0, in = skip; in < len; in++, pos++)
    {
        if (pos == 3)
        {
            pos = 0;
            Out[idx++] = delimiter;
        }
        Out[idx++] = buf[in];
    }

    Out[idx] = 0;

    return Out;
}

const char* isEnabled(bool enabled)
{
    return enabled ? "enabled" : "disabled";
}
