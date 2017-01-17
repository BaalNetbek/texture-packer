/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

struct Bitmap;

class cImageSaver final
{
public:
    cImageSaver(const Bitmap& bitmap);
    ~cImageSaver();

    bool save(const char* filename) const;

protected:
    const Bitmap& m_bitmap;
};
