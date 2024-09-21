/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "Types/Types.h"

struct sConfig;

class cAtlasSize final
{
public:
    cAtlasSize(const sConfig& config);

    void addRect(const sSize& size);
    uint32_t getArea() const;

    sSize calcSize() const;
    sSize nextSize(const sSize& size, uint32_t step) const;
    bool isGood(const sSize& size) const;

public:
    static uint32_t NextPot(uint32_t i);
    static uint32_t FixSize(uint32_t size, bool isPot);

private:
    const sConfig& m_config;

private:
    sSize m_maxRectSize;
    uint32_t m_area = 0u;
};
