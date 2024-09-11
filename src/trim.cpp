/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "Trim.h"
#include "Config.h"
#include "Utils.h"

#include <cstdio>

bool cTrim::trim(const char* path, const cBitmap& input)
{
    m_bitmap.clear();

    auto left = findLeft(input);
    auto right = findRigth(input);
    auto top = findTop(input);
    auto bottom = findBottom(input);

    auto& size = input.getSize();
    if (left == 0 && right == size.width && top == 0 && bottom == size.height)
    {
        return false;
    }

    if (left > right || top > bottom)
    {
        printf("(WW) Empty sprite: %s\n", path);
    }
    else
    {
        m_offset.x = left;
        m_offset.y = top;

        // printf("Trim %s (%u x %u): %u <-> %u , %u <-> %u\n", path, size.width, size.height, left, right, top, bottom);

        auto src = input.getData() + top * size.width;

        m_bitmap.createBitmap({ right - left + 1, bottom - top + 1 });
        auto dst = m_bitmap.getData();

        for (uint32_t y = top; y <= bottom; y++)
        {
            for (uint32_t x = left; x <= right; x++)
            {
                *dst++ = src[x];
            }
            src += size.width;
        }
    }

    return true;
}

uint32_t cTrim::findLeft(const cBitmap& input) const
{
    auto& size = input.getSize();
    for (uint32_t x = 0u; x < size.width; x++)
    {
        auto src = input.getData() + x;
        for (uint32_t y = 0u; y < size.height; y++)
        {
            if (src->a != 0)
            {
                return x;
            }
            src += size.width;
        }
    }

    return size.width;
}

uint32_t cTrim::findRigth(const cBitmap& input) const
{
    auto& size = input.getSize();
    for (uint32_t x = 0; x < size.width; x++)
    {
        uint32_t offset = size.width - x - 1;
        auto src = input.getData() + offset;
        for (uint32_t y = 0; y < size.height; y++)
        {
            if (src->a != 0)
            {
                return offset + 1;
            }
            src += size.width;
        }
    }

    return 0;
}

uint32_t cTrim::findTop(const cBitmap& input) const
{
    auto& size = input.getSize();
    for (uint32_t y = 0; y < size.height; y++)
    {
        auto src = input.getData() + y * size.width;
        for (uint32_t x = 0; x < size.width; x++)
        {
            if (src->a != 0)
            {
                return y;
            }
            src++;
        }
    }

    return size.height;
}

uint32_t cTrim::findBottom(const cBitmap& input) const
{
    auto& size = input.getSize();
    for (uint32_t y = 0; y < size.height; y++)
    {
        uint32_t offset = size.height - y - 1;
        auto src = input.getData() + offset * size.width;
        for (uint32_t x = 0; x < size.width; x++)
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

    auto& size = input.getSize();
    if (width == size.width && height == size.height)
    {
        return false;
    }

    // printf("Trim %s %u x %u -> %u x %u \n", path, size.width, size.height, width, height);

    auto src = input.getData();

    m_bitmap.createBitmap({ width, height });
    auto dst = m_bitmap.getData();

    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
            *dst++ = src[x];
        }
        src += size.width;
    }

    return true;
}
