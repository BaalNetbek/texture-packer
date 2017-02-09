/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <string>

struct Bitmap;

class cImageSaver final
{
public:
    cImageSaver(const Bitmap& bitmap, const char* filename);
    ~cImageSaver();

    const char* getAtlasName() const
    {
        return m_filename.c_str();
    }

    bool save() const;

private:
    enum class Type
    {
        png,
        bmp,
        tga,
        unknown,
    };

    Type getWriter(const char* filename) const;

private:
    const Bitmap& m_bitmap;
    std::string m_filename;

    Type m_type;
};
