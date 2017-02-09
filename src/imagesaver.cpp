/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "imagesaver.h"
#include "bitmap.h"
#include "file.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include <algorithm>
#include <cstring>

cImageSaver::cImageSaver(const Bitmap& bitmap, const char* filename)
    : m_bitmap(bitmap)
    , m_filename(filename)
{
    auto type = getWriter(filename);
    if (type == Type::unknown)
    {
        m_filename += ".png";
        m_type = Type::png;
    }
}

cImageSaver::~cImageSaver()
{
}

cImageSaver::Type cImageSaver::getWriter(const char* filename) const
{
    const auto point = ::strrchr(filename, '.');
    if (point != nullptr)
    {
        std::string ext = point;
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        struct sCompare
        {
            Type type;
            const char* ext;
        };

        static const sCompare List[] =
        {
            { Type::png, ".png" },
            { Type::bmp, ".bmp" },
            { Type::tga, ".tga" },
            { Type::tga, ".targa" },
        };

        for (auto& t : List)
        {
            if (::strcmp(t.ext, ext.c_str()) == 0)
            {
                return t.type;
            }
        }
    }

    return Type::unknown;
}

bool cImageSaver::save() const
{
    const int w = m_bitmap.width;
    const int h = m_bitmap.height;
    const int stride = w * 4;
    const auto data = m_bitmap.data.data();

    auto filename = m_filename.c_str();

    switch (m_type)
    {
    case Type::png:
        return stbi_write_png(filename, w, h, 4, data, stride) != 0;

    case Type::bmp:
        return stbi_write_bmp(filename, w, h, 4, data) != 0;

    case Type::tga:
        return stbi_write_tga(filename, w, h, 4, data) != 0;
    }

    return false;
}
