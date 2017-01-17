/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "trim.h"

namespace
{

    unsigned findLeft(const Bitmap& bitmap)
    {
        for (unsigned x = 0; x < bitmap.width; x++)
        {
            auto src = bitmap.data.data() + x;
            for (unsigned y = 0; y < bitmap.height; y++)
            {
                if (src->a != 0)
                {
                    return x;
                }
                src += bitmap.width;
            }
        }

        return 0;
    }

    unsigned findRigth(const Bitmap& bitmap)
    {
        for (unsigned x = 0; x < bitmap.width; x++)
        {
            unsigned b = bitmap.width - x - 1;
            auto src = bitmap.data.data() + b;
            for (unsigned y = 0; y < bitmap.height; y++)
            {
                if (src->a != 0)
                {
                    return x;
                }
                src += bitmap.width;
            }
        }

        return 0;
    }

    unsigned findTop(const Bitmap& bitmap)
    {
        for (unsigned y = 0; y < bitmap.height; y++)
        {
            auto src = bitmap.data.data() + y * bitmap.width;
            for (unsigned x = 0; x < bitmap.width; x++)
            {
                if (src->a != 0)
                {
                    return y;
                }
                src++;
            }
        }

        return 0;
    }

    unsigned findBottom(const Bitmap& bitmap)
    {
        for (unsigned y = 0; y < bitmap.height; y++)
        {
            unsigned b = bitmap.height - y - 1;
            auto src = bitmap.data.data() + b * bitmap.width;
            for (unsigned x = 0; x < bitmap.width; x++)
            {
                if (src->a != 0)
                {
                    return y;
                }
                src++;
            }
        }

        return 0;
    }

}

cTrim::cTrim(unsigned border)
    : m_border(border)
{
}

bool cTrim::trim(const char* name, const Bitmap& bitmap)
{
    m_bitmap.clear();

    auto left = findLeft(bitmap);
    auto right = findRigth(bitmap);
    auto top = findTop(bitmap);
    auto bottom = findBottom(bitmap);

    unsigned border = m_border;
    if (left == border && right == border && top == border && bottom == border)
    {
        return false;
    }

    // printf("Trim %s : %u , %u , %u , %u\n", name, left, right, top, bottom);

    auto srcWidth = bitmap.width - left - right;
    auto srcHeight = bitmap.height - top - bottom;
    auto src = bitmap.data.data() + top * bitmap.width + left;

    m_bitmap.width = srcWidth + border * 2;
    m_bitmap.height = srcHeight + border * 2;
    m_bitmap.data.resize(m_bitmap.width * m_bitmap.height);
    auto dst = m_bitmap.data.data() + border * m_bitmap.width + border;

    for (unsigned y = top; y < srcHeight; y++)
    {
        for (unsigned x = 0; x < srcWidth; x++)
        {
            dst[x] = src[x];
        }
        dst += m_bitmap.width;
        src += bitmap.width;
    }

    return true;
}
