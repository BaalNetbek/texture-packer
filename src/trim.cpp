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

bool cTrim::trim(const char* path, const cBitmap& input)
{
    m_bitmap.clear();

    auto left = findLeft(input);
    auto right = findRigth(input);
    auto top = findTop(input);
    auto bottom = findBottom(input);

    if (left == 0 && right == input.getWidth() && top == 0 && bottom == input.getHeight())
    {
        return false;
    }

    if (left > right || top > bottom)
    {
        printf("(WW) Empty sprite: %s\n", path);
    }
    else
    {
        // printf("Trim %s (%u x %u): %u <-> %u , %u <-> %u\n", path, input.getWidth(), input.getHeight(), left, right, top, bottom);

        auto src = input.getData() + top * input.getWidth();

        m_bitmap.createBitmap(right - left + 1, bottom - top + 1);
        auto dst = m_bitmap.getData();

        for (uint32_t y = top; y <= bottom; y++)
        {
            for (uint32_t x = left; x <= right; x++)
            {
                *dst++ = src[x];
            }
            src += input.getWidth();
        }
    }

    return true;
}

uint32_t cTrim::findLeft(const cBitmap& input) const
{
    for (uint32_t x = 0; x < input.getWidth(); x++)
    {
        auto src = input.getData() + x;
        for (uint32_t y = 0; y < input.getHeight(); y++)
        {
            if (src->a != 0)
            {
                return x;
            }
            src += input.getWidth();
        }
    }

    return input.getWidth();
}

uint32_t cTrim::findRigth(const cBitmap& input) const
{
    for (uint32_t x = 0; x < input.getWidth(); x++)
    {
        uint32_t offset = input.getWidth() - x - 1;
        auto src = input.getData() + offset;
        for (uint32_t y = 0; y < input.getHeight(); y++)
        {
            if (src->a != 0)
            {
                return offset + 1;
            }
            src += input.getWidth();
        }
    }

    return 0;
}

uint32_t cTrim::findTop(const cBitmap& input) const
{
    for (uint32_t y = 0; y < input.getHeight(); y++)
    {
        auto src = input.getData() + y * input.getWidth();
        for (uint32_t x = 0; x < input.getWidth(); x++)
        {
            if (src->a != 0)
            {
                return y;
            }
            src++;
        }
    }

    return input.getHeight();
}

uint32_t cTrim::findBottom(const cBitmap& input) const
{
    for (uint32_t y = 0; y < input.getHeight(); y++)
    {
        uint32_t offset = input.getHeight() - y - 1;
        auto src = input.getData() + offset * input.getWidth();
        for (uint32_t x = 0; x < input.getWidth(); x++)
        {
            if (src->a != 0)
            {
                return offset + 1;
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

bool cTrimRigthBottom::trim(const char* /*path*/, const cBitmap& input)
{
    m_bitmap.clear();

    const auto border = m_config.border;

    const auto width = fixSize(findRigth(input) + border, m_config.pot);
    const auto height = fixSize(findBottom(input) + border, m_config.pot);

    if (width == input.getWidth() && height == input.getHeight())
    {
        return false;
    }

    // printf("Trim %s %u x %u -> %u x %u \n", path, input.getWidth(), input.getHeight(), width, height);

    auto src = input.getData();

    m_bitmap.createBitmap(width, height);
    auto dst = m_bitmap.getData();

    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
            *dst++ = src[x];
        }
        src += input.getWidth();
    }

    return true;
}
