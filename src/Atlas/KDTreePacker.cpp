/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "KDTreePacker.h"
#include "Config.h"
#include "Image.h"
#include "KDNode.h"

KDTreePacker::KDTreePacker(const sConfig& config)
    : AtlasPacker(config)
{
}

KDTreePacker::~KDTreePacker(void)
{
    delete m_root;
}

bool KDTreePacker::Compare(const cImage* a, const cImage* b)
{
    auto& bmpa = a->getBitmap();
    auto& sizea = bmpa.getSize();

    auto& bmpb = b->getBitmap();
    auto& sizeb = bmpb.getSize();

#if 0

    return (sizea.width > sizeb.height)
        || (sizea.width * sizea.height > sizeb.width * sizeb.height);

#else

    auto maxa = std::max(sizea.width, sizea.height);
    auto maxb = std::max(sizeb.width, sizeb.height);

    if (maxa > maxb)
    {
        return true;
    }
    if (maxb < maxa)
    {
        return false;
    }

#if 0

    // ./test.sh test-wh -max 3000 -overlay
    // Out of a total of 33 files, 30 packed better + 0 unchanged, 3 packed worse.
    // The total pixel difference across all files is -9,438,568.
    //
    // ./test.sh test-wz -max 3000 -overlay
    // Out of a total of 29 files, 26 packed better + 1 unchanged, 2 packed worse.
    // The total pixel difference across all files is -5,126,036.

    if (sizea.height > sizeb.height)
    {
        return true;
    }
    if (sizea.height < sizeb.height)
    {
        return false;
    }

    return sizea.width * sizea.height > sizeb.width * sizeb.height;

#else

    // ./test.sh test-wh -max 3000 -overlay
    // Out of a total of 33 files, 32 packed better + 0 unchanged, 1 packed worse.
    // The total pixel difference across all files is -8,871,488.
    //
    // ./test.sh test-wz -max 3000 -overlay
    // Out of a total of 29 files, 26 packed better + 1 unchanged, 2 packed worse.
    // The total pixel difference across all files is -5,372,264.

    auto areaa = sizea.width * sizea.height;
    auto areab = sizeb.width * sizeb.height;
    if (areaa > areab)
    {
        return true;
    }
    if (areaa < areab)
    {
        return false;
    }

    return sizea.height > sizeb.height;

#endif

#endif
}

void KDTreePacker::setSize(const sSize& size)
{
    const auto border = m_config.border;

    delete m_root;
    m_root = new cKDNode({ border, border, size.width - border, size.height - border }, m_config.padding);

    m_nodes.clear();
    m_atlas.createBitmap(size);
}

bool KDTreePacker::add(const cImage* image)
{
    auto& bmp = image->getBitmap();
    auto& size = bmp.getSize();
    auto node = m_root->add(size);
    if (node != nullptr)
    {
        m_nodes.push_back({ image, node });

        return true;
    }

    return false;
}

void KDTreePacker::makeAtlas(bool overlay)
{
    for (const auto& piece : m_nodes)
    {
        auto rc = piece.node->getRect();
        copyBitmap(rc, piece.image, overlay);
    }
}

uint32_t KDTreePacker::getRectsCount() const
{
    return (uint32_t)m_nodes.size();
}

const cImage* KDTreePacker::getImageByIndex(uint32_t idx) const
{
    return m_nodes[idx].image;
}

const sRect& KDTreePacker::getRectByIndex(uint32_t idx) const
{
    return m_nodes[idx].node->getRect();
}
