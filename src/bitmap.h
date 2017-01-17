/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <vector>

struct Bitmap
{
    void clear()
    {
        width = 0;
        height = 0;
        data.clear();
    }

    void setSize(unsigned w, unsigned h)
    {
        width = w;
        height = h;
        data.resize(width * height);
    }

    struct Pixel
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };

    unsigned width = 0;
    unsigned height = 0;
    std::vector<Pixel> data;
};
