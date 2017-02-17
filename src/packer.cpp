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

        out << "<objects>\n";
        for (uint32_t i = 0, size = m_packer->getRectsCount(); i < size; i++)
        {
            auto& name = m_packer->getImageByIndex(i)->getName();
            const auto& rc = m_packer->getRectByIndex(i);
            const auto tx = rc.left;
            const auto ty = rc.top;
            const auto tw = rc.width();
            const auto th = rc.height();

            out << "    ";
            out << "<sprite id=\"" << name << "\" texture=\"" << atlasName << "\" ";
            out << "rect=\"" << tx << " " << ty << " " << tw << " " << th << "\" ";
            out << "hotspot=\"" << (tw * 0.5f) << " " << (th * 0.5f) << "\" />\n";
        }
        out << "</objects>\n";

        file.write((void*)out.str().c_str(), out.str().length());

        return true;
    }

    return false;
}
