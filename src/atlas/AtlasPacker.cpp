/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "AtlasPacker.h"
#include "KDTreePacker.h"
#include "SimplePacker.h"
#include "config.h"
#include "file.h"
#include "image.h"
#include "trim.h"
#include "types/rect.h"
#include <algorithm>
#include <cmath>
#include <sstream>

AtlasPacker* AtlasPacker::create(size_t count, const sConfig& config)
{
    AtlasPacker* packer = nullptr;

    if (config.slowMethod)
    {
        packer = new SimplePacker(count, config);
    }
    else
    {
        packer = new KDTreePacker(config);
    }

    return packer;
}

AtlasPacker::AtlasPacker(const sConfig& config)
    : m_config(config)
{
}

AtlasPacker::~AtlasPacker()
{
}

void AtlasPacker::copyBitmap(const sRect& rc, const cImage* image, bool overlay)
{
    auto& bmp = image->getBitmap();

    const auto width = bmp.getWidth();
    const auto height = bmp.getHeight();

    const auto padding = m_config.padding;

    const auto offx = rc.left;
    const auto offy = rc.top;
    const auto offxPadded = offx + padding;
    const auto offyPadded = offy + padding;
    const auto pitch = m_atlas.getWidth();

    auto srcData = bmp.getData();
    auto dstData = m_atlas.getData();

    for (uint32_t y = 0; y < height; y++)
    {
        auto dst = dstData + (y + offyPadded) * pitch + offxPadded;
        for (uint32_t x = 0; x < width; x++)
        {
            *dst++ = *srcData++;
        }
    }

    if (padding > 0)
    {
        // adding the border
        /*
          Consider the following picture:
          ++======++
          ++======++
          ||oooooo||
          ||oooooo||
          ||oooooo||
          ||oooooo||
          ++======++
          ++======++

          o - image pixels, copied in the previous loop
          | - left and right border
          = - top and bottom border
          + - corner pixels
        */

        auto srcLeft = bmp.getData();
        for (uint32_t y = 0; y < height; ++y)
        {
            auto left = dstData + (y + offyPadded) * pitch + offx;
            auto right = dstData + (y + offyPadded) * pitch + offxPadded + width;
            for (size_t i = 0; i < padding; ++i)
            {
                *(left + i) = *srcLeft;
                *(right + i) = *(srcLeft + width - 1);
            }
            srcLeft += width;
        }

        auto srcTop = bmp.getData();
        for (uint32_t x = 0; x < width; ++x)
        {
            auto top = dstData + offy * pitch + offxPadded + x;
            auto bottom = dstData + (offyPadded + height) * pitch + offxPadded + x;
            for (size_t i = 0; i < padding; ++i)
            {
                *(top + pitch * i) = *srcTop;
                *(bottom + pitch * i) = *(srcTop + width * (height - 1));
            }
            srcTop++;
        }

        // border corner pixels
        {
            auto dst = dstData + offx + offy * pitch;
            auto widthOffset = width + padding;
            auto heightOffset = height + padding;

            auto topLeft = dst;
            auto topRight = dst + widthOffset;
            auto bottomLeft = dst + pitch * heightOffset;
            auto bottomRight = dst + pitch * heightOffset + widthOffset;

            auto src = dstData + offyPadded * pitch + offxPadded;

            auto tlValue = *src;
            auto trValue = *(src + width - 1);
            auto blValue = *(src + pitch * (height - 1));
            auto brValue = *(src + pitch * (height - 1) + width - 1);
            for (size_t x = 0; x < padding; ++x)
            {
                for (size_t y = 0; y < padding; ++y)
                {
                    auto c = x * pitch + y;

                    *(topLeft + c) = tlValue;
                    *(topRight + c) = trValue;
                    *(bottomLeft + c) = blValue;
                    *(bottomRight + c) = brValue;
                }
            }
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
            auto dst = dstData + (y + offy) * pitch + offx;
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

                *dst++ = {
                    static_cast<uint8_t>(r * 255.0f),
                    static_cast<uint8_t>(g * 255.0f),
                    static_cast<uint8_t>(b * 255.0f),
                    static_cast<uint8_t>(a * 255.0f)
                };
            }
        }
    }
}

void AtlasPacker::buildAtlas()
{
    makeAtlas(m_config.overlay);

    cTrimRigthBottom trim(m_config);
    if (trim.trim("atlas", m_atlas))
    {
        m_atlas = std::move(trim.getBitmap());
    }
}

bool AtlasPacker::generateResFile(const char* name, const char* atlasName)
{
    cFile file;
    if (file.open(name, "w"))
    {
        std::stringstream out;

        const uint32_t size = getRectsCount();
        std::vector<uint32_t> indexes(size);
        for (uint32_t i = 0; i < size; i++)
        {
            indexes[i] = i;
        }

        std::sort(indexes.begin(), indexes.end(), [this](uint32_t a, uint32_t b) {
            auto& na = getImageByIndex(a)->getName();
            auto& nb = getImageByIndex(b)->getName();
            return na < nb;
        });

        out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

        auto& bitmap = m_atlas;
        out << "<atlas width=\"" << bitmap.getWidth() << "\" height=\"" << bitmap.getHeight() << "\">\n";

        for (uint32_t i = 0; i < size; i++)
        {
            const uint32_t idx = indexes[i];

            auto& spriteId = getImageByIndex(idx)->getSpriteId();
            const auto& rc = getRectByIndex(idx);
            const auto tx = rc.left;
            const auto ty = rc.top;
            const auto tw = rc.width();
            const auto th = rc.height();

            out << "    ";
            out << "<" << spriteId << " texture=\"" << atlasName << "\" ";
            out << "rect=\"" << tx + m_config.padding << " " << ty + m_config.padding << " " << tw << " " << th << "\" ";
            out << "hotspot=\"" << std::round(tw * 0.5f) << " " << std::round(th * 0.5f) << "\" />\n";
        }
        out << "</atlas>\n";

        file.write((void*)out.str().c_str(), out.str().length());

        return true;
    }

    return false;
}
