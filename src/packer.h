/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "types/bitmap.h"
#include "types/rect.h"

#include <cstddef>
#include <vector>

class AtlasPacker;
class cImage;
struct sConfig;
struct sSize;

class cPacker final
{
public:
    cPacker(size_t count, const sConfig& config);
    ~cPacker();

    bool compare(const cImage* a, const cImage* b) const;

    void setSize(const sSize& size);
    bool add(const cImage* image);
    void buildAtlas();

    const sBitmap& getBitmap() const;

    bool generateResFile(const char* name, const char* atlasName);

private:
    const sConfig& m_config;
    AtlasPacker* m_packer;
};
