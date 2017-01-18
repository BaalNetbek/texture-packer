/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "bitmap.h"

class cTrim final
{
public:
    bool trim(const char* name, const Bitmap& input);

    const Bitmap& getBitmap() const
    {
        return m_bitmap;
    }

private:
    Bitmap m_bitmap;
};
