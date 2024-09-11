/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "Types/Bitmap.h"

#include <string>

class cTrim;

class cImage final
{
public:
    static bool IsImage(const char* path);

public:
    ~cImage();

    void clear();

    bool load(const char* path, uint32_t trimPath, cTrim* trim);

    const cBitmap& getBitmap() const
    {
        return m_bitmap;
    }

    const sSize& getOriginalSize() const
    {
        return m_originalSize;
    }

    const sOffset& getOffset() const
    {
        return m_offset;
    }

    const std::string& getName() const
    {
        return m_name;
    }

    const std::string& getSpriteId() const
    {
        return m_spriteId;
    }

private:
    std::string m_name;
    std::string m_spriteId;

    sSize m_originalSize;
    sOffset m_offset;

    uint8_t* m_stbImageData = nullptr;
    cBitmap m_bitmap;
};
