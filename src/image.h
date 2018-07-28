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
    static bool IsImage(const char* name);

    bool load(const char* name, cTrim* trim);

    const sBitmap& getBitmap() const
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
    sBitmap m_bitmap;
};
