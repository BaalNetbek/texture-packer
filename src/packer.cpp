/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "packer.h"
#include "atlas/KDTreePacker.h"
#include "atlas/SimplePacker.h"
#include "config.h"
#include "file.h"
#include "image.h"
#include "trim.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

cPacker::cPacker(size_t count, const sConfig& config)
    : m_config(config)
{
    if (config.slowMethod)
    {
        m_packer = new SimplePacker(count, config.padding, config.border);
    }
    else
    {
        m_packer = new KDTreePacker(config.padding, config.border);
    }
}

cPacker::~cPacker()
{
    delete m_packer;
}

bool cPacker::compare(const cImage* a, const cImage* b) const
{
    return m_packer->compare(a, b);
}

void cPacker::setSize(const sSize& size)
{
    m_packer->setSize(size);
}

bool cPacker::add(const cImage* image)
{
    return m_packer->add(image);
}

void cPacker::buildAtlas()
{
    m_packer->makeAtlas(m_config.overlay);

    cTrimRigthBottom trim(m_config);
    if (trim.trim("atlas", m_packer->getBitmap()))
    {
        m_packer->getBitmap() = trim.getBitmap();
    }
}

const sBitmap& cPacker::getBitmap() const
{
    return m_packer->getBitmap();
}

bool cPacker::generateResFile(const char* name, const char* atlasName)
{
    cFile file;
    if (file.open(name, "w"))
    {
        std::stringstream out;

        const uint32_t size = m_packer->getRectsCount();
        std::vector<uint32_t> indexes(size);
        for (uint32_t i = 0; i < size; i++)
        {
            indexes[i] = i;
        }

        std::sort(indexes.begin(), indexes.end(), [this](uint32_t a, uint32_t b) {
            auto& na = m_packer->getImageByIndex(a)->getName();
            auto& nb = m_packer->getImageByIndex(b)->getName();
            return na < nb;
        });

        out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

        auto& bitmap = getBitmap();
        out << "<atlas width=\"" << bitmap.width << "\" height=\"" << bitmap.height << "\">\n";

        for (uint32_t i = 0; i < size; i++)
        {
            const uint32_t idx = indexes[i];

            auto& spriteId = m_packer->getImageByIndex(idx)->getSpriteId();
            const auto& rc = m_packer->getRectByIndex(idx);
            const auto tx = rc.left;
            const auto ty = rc.top;
            const auto tw = rc.width();
            const auto th = rc.height();

            out << "    ";
            out << "<" << spriteId << " texture=\"" << atlasName << "\" ";
            out << "rect=\"" << tx << " " << ty << " " << tw << " " << th << "\" ";
            out << "hotspot=\"" << std::round(tw * 0.5f) << " " << std::round(th * 0.5f) << "\" />\n";
        }
        out << "</atlas>\n";

        file.write((void*)out.str().c_str(), out.str().length());

        return true;
    }

    return false;
}
