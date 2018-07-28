/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "image.h"
#include "file.h"
#include "trim.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <cstring>

bool cImage::IsImage(const char* name)
{
    cFile file;
    if (file.open(name))
    {
        return true;
    }

    return false;
}

bool cImage::load(const char* name, cTrim* trim)
{
    m_name = name;

    m_spriteId = name;
    auto pos = m_spriteId.find_last_of(".");
    if (pos != std::string::npos)
    {
        m_spriteId.erase(pos);
    }

    //     N=#comp     components
    //       1           grey
    //       2           grey, alpha
    //       3           red, green, blue
    //       4           red, green, blue, alpha
    int width;
    int height;
    int bpp;
    const auto data = stbi_load(name, &width, &height, &bpp, 4);

    if (data != nullptr)
    {
        m_bitmap.setSize(width, height);
        auto bitmap = m_bitmap.data.data();
        auto in = data;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                *bitmap = { in[0], in[1], in[2], in[3] };
                in += 4;
                bitmap++;
            }
        }
        stbi_image_free(data);

        if (trim != nullptr)
        {
            if (trim->trim(name, m_bitmap))
            {
                m_bitmap = trim->getBitmap();
            }
        }

        return true;
    }

    return false;
}
