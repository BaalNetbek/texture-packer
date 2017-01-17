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
    cTrim(unsigned border);

    bool trim(const char* name, const Bitmap& input);

    const Bitmap& getBitmap() const
    {
        return m_bitmap;
    }

private:
    unsigned m_border;
    Bitmap m_bitmap;
};
