/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "../types/bitmap.h"

class cImage;
struct sSize;
struct sRect;

class AtlasPacker
{
public:
    AtlasPacker(uint32_t padding, uint32_t border);
    virtual ~AtlasPacker();

    virtual bool compare(const cImage* a, const cImage* b) const = 0;

    virtual void setSize(const sSize& size) = 0;
    virtual bool add(const cImage* image) = 0;
    virtual void makeAtlas(bool overlay) = 0;

    const sBitmap& getBitmap() const
    {
        return m_atlas;
    }

    sBitmap& getBitmap()
    {
        return m_atlas;
    }

    virtual uint32_t getRectsCount() const = 0;
    virtual const cImage* getImageByIndex(uint32_t idx) const = 0;
    virtual const sRect& getRectByIndex(uint32_t idx) const = 0;

protected:
    void copyBitmap(const sRect& rc, const cImage* image, bool overlay);

protected:
    uint32_t m_border;
    uint32_t m_padding;
    sBitmap m_atlas;
};
