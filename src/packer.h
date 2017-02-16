/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "bitmap.h"
#include "rect.h"

#include <cstddef>
#include <vector>

class cImage;
struct sSize;

class cPacker final
{
public:
    cPacker(size_t count, uint32_t border, uint32_t padding);
    ~cPacker();

    void setSize(const sSize& size);
    bool add(const cImage* image);
    void fillTexture(bool overlay);
    bool generateResFile(const char* name, const char* atlasName);

    const Bitmap& getBitmap() const
    {
        return m_atlas;
    }

private:
    void clear();
    const sRect* checkRegion(const sRect& region) const;
    void copyBitmap(const sRect& test, const cImage* image, bool overlay);

private:
    uint32_t m_border;
    uint32_t m_padding;
    Bitmap m_atlas;

    struct sImage
    {
        const cImage* image;
        sRect rc;
    };
    std::vector<sImage> m_images;
};
