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

    bool intersect(const sRect& rc) const
    {
        return (left < rc.right && right > rc.left && top < rc.bottom && bottom > rc.top);
    }

    unsigned left;
    unsigned right;
    unsigned top;
    unsigned bottom;
};
