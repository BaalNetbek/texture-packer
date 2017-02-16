/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

struct sSize
{
    sSize() = default;

    sSize(unsigned w, unsigned h)
        : width(w)
        , height(h)
    {
    }

    unsigned width = 0;
    unsigned height = 0;
};
