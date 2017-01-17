/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "bitmap.h"

#include <string>

class cTrim;

class cImage final
{
public:
    static bool IsImage(const char* name);

    bool load(const char* name, cTrim* trim);

    const Bitmap& getBitmap() const
    {
        return m_bitmap;
    }

    const char* getName() const
    {
        return m_name.c_str();
    }

protected:
    std::string m_name;
    Bitmap m_bitmap;
};
