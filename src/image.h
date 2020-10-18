/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "types/bitmap.h"

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

    uint8_t* m_stbImageData = nullptr;
    cBitmap m_bitmap;
};
