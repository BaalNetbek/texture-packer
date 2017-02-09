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

cImageSaver::cImageSaver(const Bitmap& bitmap)
    : m_bitmap(bitmap)
{
}

cImageSaver::~cImageSaver()
{
}

bool cImageSaver::save(const char* filename) const
{
    const int w = m_bitmap.width;
    const int h = m_bitmap.height;
    const int stride = w * 4;
    const auto data = m_bitmap.data.data();
    return stbi_write_png(filename, w, h, 4, data, stride) != 0;
}
