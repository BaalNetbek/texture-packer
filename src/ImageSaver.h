/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <string>

class cBitmap;

class cImageSaver final
{
public:
    cImageSaver(const cBitmap& bitmap, const std::string& filename);
    ~cImageSaver();

    const std::string& getAtlasName() const
    {
        return m_filename;
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

    Type getWriter(const std::string& filename) const;

private:
    const cBitmap& m_bitmap;
    std::string m_filename;

private:
    Type m_type;
};
