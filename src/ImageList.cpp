/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "ImageList.h"
#include "Atlas/AtlasPacker.h"
#include "Config.h"
#include "Image.h"
#include "Trim.h"

cImageList::cImageList(const sConfig& config, uint32_t reserve)
    : m_trim(config.trim
                 ? new cTrim()
                 : nullptr)
{
    m_images.reserve(reserve);
}

cImageList::~cImageList()
{
    for (auto img : m_images)
    {
        delete img;
    }

    delete m_trim;
}

const cImage* cImageList::loadImage(const std::string& path, uint32_t trimCount)
{
    if (cImage::IsImage(path.c_str()))
    {
        std::unique_ptr<cImage> image(new cImage());

        if (image->load(path.c_str(), trimCount, m_trim) == true)
        {
            m_images.push_back(image.release());

            return m_images.back();
        }
    }

    return nullptr;
}

void cImageList::sort(AtlasPacker* packer)
{
    std::stable_sort(m_images.begin(), m_images.end(), [&packer](const cImage* a, const cImage* b) -> bool {
        return packer->compare(a, b);
    });
}

const cImageList::List& cImageList::getList() const
{
    return m_images;
}
