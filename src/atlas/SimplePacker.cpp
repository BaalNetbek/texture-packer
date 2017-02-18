/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "SimplePacker.h"
#include "../image.h"
#include "../types/size.h"

SimplePacker::SimplePacker(uint32_t count, uint32_t padding, uint32_t border)
    : AtlasPacker(padding, border)
{
    m_images.reserve(count);
}

SimplePacker::~SimplePacker()
{
}

bool SimplePacker::compare(const cImage* a, const cImage* b) const
{
    auto& bmpa = a->getBitmap();
    auto& bmpb = b->getBitmap();
    return (bmpa.width * bmpa.height > bmpb.width * bmpb.height)
        && (bmpa.width + bmpa.height > bmpb.width + bmpb.height);
}

bool SimplePacker::add(const cImage* image)
{
    const auto& bmp = image->getBitmap();

    const auto border = m_border;
    const auto padding = m_padding;

    const auto width = m_atlas.width - bmp.width - border;
    const auto height = m_atlas.height - bmp.height - border;

    sRect imgRc;

    for (uint32_t y = border; y < height;)
    {
        imgRc.top = y;
        imgRc.bottom = y + bmp.height;

        for (uint32_t x = border; x < width;)
        {
            imgRc.left = x;
            imgRc.right = x + bmp.width;

            const auto rc = checkRegion(imgRc);
            if (rc == nullptr)
            {
                // merge this region into the used region's vector
                m_images.push_back({ image, imgRc });

                return true;
            }

            x += rc->width() + padding;
        }
        y++;
    }

    return false;
}

const sRect* SimplePacker::checkRegion(const sRect& region) const
{
    const auto padding = m_padding;
    for (const auto& img : m_images)
    {
        const auto& rc = img.rc;
        if (region.left < rc.right + padding
            && region.right + padding > rc.left
            && region.top < rc.bottom + padding
            && region.bottom + padding > rc.top)
        {
            return &rc;
        }
    }

    return nullptr;
}

void SimplePacker::setSize(const sSize& size)
{
    m_images.clear();
    m_atlas.setSize(size.width, size.height);
}

void SimplePacker::makeAtlas(bool overlay)
{
    for (const auto& img : m_images)
    {
        copyBitmap(img.rc, img.image, overlay);
    }
}

uint32_t SimplePacker::getRectsCount() const
{
    return (uint32_t)m_images.size();
}

const cImage* SimplePacker::getImageByIndex(uint32_t idx) const
{
    return m_images[idx].image;
}

const sRect& SimplePacker::getRectByIndex(uint32_t idx) const
{
    return m_images[idx].rc;
}
