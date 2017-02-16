/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <cstdint>

struct sRect
{
    uint32_t width() const
    {
        return right - left;
    }

    uint32_t height() const
    {
        return bottom - top;
    }

    uint32_t left;
    uint32_t right;
    uint32_t top;
    uint32_t bottom;
};
