/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "packer.h"
#include "image.h"
#include "file.h"

#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

cPacker::cPacker(size_t count, unsigned border)
    : m_border(border)
    , m_freePixels(0)
{
    m_images.reserve(count);
}

cPacker::~cPacker()
{
    clear();
}

void cPacker::clear()
{
    m_atlas.clear();
    m_images.clear();
}

void cPacker::setSize(unsigned width, unsigned height)
{
    clear();

    m_atlas.width = width;
    m_atlas.height = height;

    m_freePixels = width * height;
    m_atlas.data.resize(m_freePixels);
}

bool cPacker::add(const cImage* image)
{
    auto& bmp = image->getBitmap();
    const unsigned width = bmp.width + m_border * 2;
    const unsigned height = bmp.height + m_border * 2;
    const unsigned pixels = width * height;

    if (pixels > m_freePixels || m_atlas.width < width || m_atlas.height < height)
    {
        return false;
    }

    sRect img_rc;

    for (unsigned y = 0, rows = m_atlas.height - height; y < rows; )
    {
        img_rc.top = y;
        img_rc.bottom = y + height;

        for (unsigned x = 0, cols = m_atlas.width - width; x < cols; )
        {
            img_rc.left = x;
            img_rc.right = x + width;

            auto rc = checkRegion(img_rc);
            if (rc == nullptr)
            {
                m_freePixels -= pixels;

                // merge this region into the used region's vector
                m_images.push_back({ image, img_rc });

                return true;
            }

            x += rc->width();
        }
        y++;
    }

    return false;
}

const sRect* cPacker::checkRegion(const sRect& region) const
{
    for (const auto& img : m_images)
    {
        if (img.rc.intersect(region))
        {
            return &img.rc;
        }
    }

    return nullptr;
}

void cPacker::fillTexture()
{
    for (const auto& img : m_images)
    {
        copyBitmap(img.rc, img.image);
    }
}

void cPacker::copyBitmap(const sRect& rc, const cImage* image)
{
    auto& bmp = image->getBitmap();
    auto src = bmp.data.data();

    const unsigned offx = rc.left + m_border;
    const unsigned offy = rc.top + m_border;
    const unsigned pitch = m_atlas.width;

    for (unsigned y = 0, height = bmp.height; y < height; y++)
    {
        auto dst = m_atlas.data.data() + (y + offy) * pitch + offx;
        for (unsigned x = 0, width = bmp.width; x < width; x++)
        {
            *dst++ = *src++;
        }
    }
}

bool cPacker::generateResFile(const char* name, const char* atlasName)
{
    cFile file;
    if (file.open(name, "w"))
    {
        std::stringstream out;
        out << "<objects>\n";
        for (const auto& img : m_images)
        {
            auto tx = img.rc.left;
            auto ty = img.rc.top;
            auto tw = img.rc.width();
            auto th = img.rc.height();
            out << "    ";
            out << "<sprite id=\"" << img.image->getName() << "\" texture=\"" << atlasName << "\" ";
            out << "rect=\"" << tx << " " << ty << " " << tw << " " << th << "\" ";
            out << "hotspot=\"" << (tw >> 1) << " " << (th >> 1) << "\" />\n";
        }
        out << "</objects>\n";
        file.write((void*)out.str().c_str(), out.str().length());

        return true;
    }

    return false;
}
