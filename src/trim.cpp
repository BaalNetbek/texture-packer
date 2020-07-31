/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "trim.h"
#include "config.h"
#include "utils.h"

#include <cstdio>

bool cTrim::trim(const char* path, const sBitmap& input)
{
    m_bitmap.clear();

    auto left = findLeft(input);
    auto right = findRigth(input);
    auto top = findTop(input);
    auto bottom = findBottom(input);

    if (left == 0 && right == input.width && top == 0 && bottom == input.height)
    {
        return false;
    }

    if (left > right || top > bottom)
    {
        printf("(WW) Empty sprite: %s\n", path);
    }
    else
    {
        // printf("Trim %s (%u x %u): %u <-> %u , %u <-> %u\n", path, input.width, input.height, left, right, top, bottom);

        auto src = input.data.data() + top * input.width;

        m_bitmap.setSize(right - left + 1, bottom - top + 1);
        auto dst = m_bitmap.data.data();

        for (uint32_t y = top; y <= bottom; y++)
        {
            for (uint32_t x = left; x <= right; x++)
            {
                *dst++ = src[x];
            }
            src += input.width;
        }
    }

    return true;
}

uint32_t cTrim::findLeft(const sBitmap& input) const
{
    for (uint32_t x = 0; x < input.width; x++)
    {
        auto src = input.data.data() + x;
        for (uint32_t y = 0; y < input.height; y++)
        {
            if (src->a != 0)
            {
                return x;
            }
            src += input.width;
        }
    }

    return input.width;
}

uint32_t cTrim::findRigth(const sBitmap& input) const
{
    for (uint32_t x = 0; x < input.width; x++)
    {
        uint32_t offset = input.width - x;
        auto src = input.data.data() + offset;
        for (uint32_t y = 0; y < input.height; y++)
        {
            if (src->a != 0)
            {
                return offset;
            }
            src += input.width;
        }
    }

    return 0;
}

uint32_t cTrim::findTop(const sBitmap& input) const
{
    for (uint32_t y = 0; y < input.height; y++)
    {
        auto src = input.data.data() + y * input.width;
        for (uint32_t x = 0; x < input.width; x++)
        {
            if (src->a != 0)
            {
                return y;
            }
            src++;
        }
    }

    return input.height;
}

uint32_t cTrim::findBottom(const sBitmap& input) const
{
    for (uint32_t y = 0; y < input.height; y++)
    {
        uint32_t offset = input.height - y - 1;
        auto src = input.data.data() + offset * input.width;
        for (uint32_t x = 0; x < input.width; x++)
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



cTrimRigthBottom::cTrimRigthBottom(const sConfig& config)
    : cTrim()
    , m_config(config)
{
}

bool cTrimRigthBottom::trim(const char* path, const sBitmap& input)
{
    m_bitmap.clear();

    const auto border = m_config.border;

    const auto width = fixSize(findRigth(input) + border, m_config.pot);
    const auto height = fixSize(findBottom(input) + border, m_config.pot);

    if (width == input.width && height == input.height)
    {
        return false;
    }

    // printf("Trim %s %u x %u -> %u x %u \n", path, input.width, input.height, width, height);

    auto src = input.data.data();

    m_bitmap.setSize(width, height);
    auto dst = m_bitmap.data.data();

    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
            *dst++ = src[x];
        }
        src += input.width;
    }

    return true;
}
