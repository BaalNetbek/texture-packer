/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <cstdint>
#include <vector>

struct sBitmap
{
    void clear()
    {
        width = 0;
        height = 0;
        data.clear();
    }

    void setSize(uint32_t w, uint32_t h)
    {
        width = w;
        height = h;
        data.resize(width * height);
    }

    struct Pixel
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    uint32_t width = 0;
    uint32_t height = 0;
    std::vector<Pixel> data;
};
