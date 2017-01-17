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

class cPacker final
{
public:
    cPacker(size_t count, unsigned border);
    ~cPacker();

    void setSize(unsigned width, unsigned height);
    bool add(const cImage* image);
    void fillTexture();
    bool generateResFile(const char* name);

    const Bitmap& getBitmap() const
    {
        return m_atlas;
    }

private:
    void clear();
    const sRect* checkRegion(const sRect& region) const;
    void copyBitmap(const sRect& test, const cImage* image);

private:
    unsigned m_border;
    unsigned m_freePixels;
    Bitmap m_atlas;

    struct sImage
    {
        const cImage* image;
        sRect rc;
    };
    std::vector<sImage> m_images;
};
