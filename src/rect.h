/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

struct sRect
{
    unsigned width() const
    {
        return right - left;
    }

    unsigned height() const
    {
        return bottom - top;
    }

    unsigned left;
    unsigned right;
    unsigned top;
    unsigned bottom;
};
