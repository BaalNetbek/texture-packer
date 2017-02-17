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

private:
    std::string m_name;
    sBitmap m_bitmap;
};
