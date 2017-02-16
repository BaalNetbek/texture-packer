/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "utils.h"

#include <cstdio>
#include <cstring>
#include <sys/time.h>

uint32_t nextPot(uint32_t i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;
    i++;
    return i;
}

uint64_t getCurrentTime()
{
    timeval t;
    ::gettimeofday(&t, 0);

    return (uint64_t)(t.tv_sec * 1000000 + t.tv_usec);
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
