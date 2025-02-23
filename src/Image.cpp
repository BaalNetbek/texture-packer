/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "Image.h"
#include "Log.h"
#include "Trim.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <algorithm>
#include <cstring>

namespace
{

    std::string TrimPath(const char* path, uint32_t trimPath)
    {
        std::string res;
        if (path != nullptr)
        {
            res = path;

            res.erase(0, trimPath);

            auto pos = res.find_last_of(".");
            if (pos != std::string::npos)
            {
                res.erase(pos);
            }

            if (res.substr(0, 2) == "./")
            {
                res.erase(0, 2);
            }

            std::replace(res.begin(), res.end(), '/', '_');
        }

        return res;
    }

} // namespace

bool cImage::IsImage(const char* path)
{
    if (path != nullptr)
    {
        std::string res = path;
        auto pos = res.find_last_of("/");
        return pos != std::string::npos
            && pos + 1 != std::string::npos
            && res[pos + 1] != '.';
    }

    return false;
}

cImage::~cImage()
{
    clear();
}

void cImage::clear()
{
    if (m_stbImageData != nullptr)
    {
        stbi_image_free(m_stbImageData);
        m_stbImageData = nullptr;
    }

    m_bitmap.clear();
}

bool cImage::load(const char* path, uint32_t trimPath, cTrim* trim)
{
    clear();

    m_name = path;

    m_spriteId = TrimPath(path, trimPath);
    if (m_spriteId.length() == 0)
    {
        cLog::Error("Trim value too big for path '{}'.", path);
        return false;
    }

    // N=#comp | components
    // --------+------------------------
    // 1       | grey
    // 2       | grey, alpha
    // 3       | red, green, blue
    // 4       | red, green, blue, alpha
    int width;
    int height;
    int bpp;
    m_stbImageData = stbi_load(path, &width, &height, &bpp, 4);

    m_originalSize = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };

    m_bitmap.setBitmap(m_originalSize, m_stbImageData);

    if (m_stbImageData != nullptr && trim != nullptr)
    {
        if (trim->trim(path, m_bitmap))
        {
            m_bitmap = std::move(trim->getBitmap());
            m_offset = trim->getOffset();
        }
    }

    return m_stbImageData != nullptr;
}
