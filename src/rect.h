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
    int width() const
    {
        return right - left;
    }

    int height() const
    {
        return bottom - top;
    }

    bool intersect(const sRect& region) const;

    int left;
    int right;
    int top;
    int bottom;
};
