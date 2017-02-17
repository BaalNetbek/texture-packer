/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "AtlasPacker.h"
#include "../image.h"
#include "../types/rect.h"

AtlasPacker::AtlasPacker(uint32_t padding, uint32_t border)
    : m_border(border)
    , m_padding(padding)
{
}

AtlasPacker::~AtlasPacker()
{
}

void AtlasPacker::copyBitmap(const sRect& rc, const cImage* image, bool overlay)
{
    auto& bmp = image->getBitmap();

    const auto width = bmp.width;
    const auto height = bmp.height;

    const auto offx = rc.left;
    const auto offy = rc.top;
    const auto pitch = m_atlas.width;

    auto src = bmp.data.data();

    for (uint32_t y = 0; y < height; y++)
    {
        auto dst = m_atlas.data.data() + (y + offy) * pitch + offx;
        for (uint32_t x = 0; x < width; x++)
        {
            *dst++ = *src++;
        }
    }

    if (overlay)
    {
        const float sR = 0.0f;
        const float sG = 1.0f;
        const float sB = 0.0f;
        const float sA = 0.6f;
        const float inv = 1.0f / 255.0f;

        for (uint32_t y = 0; y < height; y++)
        {
            auto dst = m_atlas.data.data() + (y + offy) * pitch + offx;
            for (uint32_t x = 0; x < width; x++)
            {
                const float dR = dst->r * inv;
                const float dG = dst->g * inv;
                const float dB = dst->b * inv;
                const float dA = dst->a * inv;

                const float r = sA * (sR - dR) + dR;
                const float g = sA * (sG - dG) + dG;
                const float b = sA * (sB - dB) + dB;
                const float a = dA * (1.0f - sA) + sA;

                *dst++ =
                {
                    static_cast<uint8_t>(r * 255.0f),
                    static_cast<uint8_t>(g * 255.0f),
                    static_cast<uint8_t>(b * 255.0f),
                    static_cast<uint8_t>(a * 255.0f)
                };
            }
        }
    }
}
