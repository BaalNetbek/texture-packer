/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "trim.h"

#include <cstdio>

namespace
{

    uint32_t findLeft(const Bitmap& bitmap)
    {
        for (uint32_t x = 0; x < bitmap.width; x++)
        {
            auto src = bitmap.data.data() + x;
            for (uint32_t y = 0; y < bitmap.height; y++)
            {
                if (src->a != 0)
                {
                    return x;
                }
                src += bitmap.width;
            }
        }

        return bitmap.width;
    }

    uint32_t findRigth(const Bitmap& bitmap)
    {
        for (uint32_t x = 0; x < bitmap.width; x++)
        {
            uint32_t offset = bitmap.width - x - 1;
            auto src = bitmap.data.data() + offset;
            for (uint32_t y = 0; y < bitmap.height; y++)
            {
                if (src->a != 0)
                {
                    return offset;
                }
                src += bitmap.width;
            }
        }

        return 0;
    }

    uint32_t findTop(const Bitmap& bitmap)
    {
        for (uint32_t y = 0; y < bitmap.height; y++)
        {
            auto src = bitmap.data.data() + y * bitmap.width;
            for (uint32_t x = 0; x < bitmap.width; x++)
            {
                if (src->a != 0)
                {
                    return y;
                }
                src++;
            }
        }

        return bitmap.height;
    }

    uint32_t findBottom(const Bitmap& bitmap)
    {
        for (uint32_t y = 0; y < bitmap.height; y++)
        {
            uint32_t offset = bitmap.height - y - 1;
            auto src = bitmap.data.data() + offset * bitmap.width;
            for (uint32_t x = 0; x < bitmap.width; x++)
            {
                if (src->a != 0)
                {
                    return offset;
                }
                src++;
            }
        }

        return 0;
    }

}

bool cTrim::trim(const char* name, const Bitmap& bitmap)
{
    m_bitmap.clear();

    auto left = findLeft(bitmap);
    auto right = findRigth(bitmap);
    auto top = findTop(bitmap);
    auto bottom = findBottom(bitmap);

    if (left == 0 && right == bitmap.width && top == 0 && bottom == bitmap.height)
    {
        return false;
    }

    if (left > right || top > bottom)
    {
        printf("(WW) Empty sprite: %s\n", name);
    }
    else
    {
        // printf("Trim %s (%u x %u): %u <-> %u , %u <-> %u\n", name, bitmap.width, bitmap.height, left, right, top, bottom);

        auto src = bitmap.data.data() + top * bitmap.width;

        m_bitmap.setSize(right - left + 1, bottom - top + 1);
        auto dst = m_bitmap.data.data();

        for (uint32_t y = top; y <= bottom; y++)
        {
            for (uint32_t x = left; x <= right; x++)
            {
                *dst++ = src[x];
            }
            src += bitmap.width;
        }
    }

    return true;
}
