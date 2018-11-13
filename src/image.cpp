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

#include <algorithm>
#include <cstring>

bool cImage::IsImage(const char* /*path*/)
{
    return true;
}

bool cImage::load(const char* path, uint32_t trimPath, cTrim* trim)
{
    m_name = path;

    m_spriteId = path;

    m_spriteId.erase(0, trimPath);

    auto pos = m_spriteId.find_last_of(".");
    if (pos != std::string::npos)
    {
        m_spriteId.erase(pos);
    }

    if (m_spriteId.substr(0, 2) == "./")
    {
        m_spriteId.erase(0, 2);
    }

    std::replace(m_spriteId.begin(), m_spriteId.end(), '/', '_');

    if (m_spriteId.length() == 0)
    {
        ::printf("(EE) Trim value too big for path '%s'\n", path);
    }

    //     N=#comp     components
    //       1           grey
    //       2           grey, alpha
    //       3           red, green, blue
    //       4           red, green, blue, alpha
    int width;
    int height;
    int bpp;
    const auto data = stbi_load(path, &width, &height, &bpp, 4);

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
            if (trim->trim(path, m_bitmap))
            {
                m_bitmap = trim->getBitmap();
            }
        }

        return true;
    }

    return false;
}
