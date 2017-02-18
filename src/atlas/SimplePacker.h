/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "AtlasPacker.h"
#include "../types/rect.h"
#include "../types/rect.h"

class SimplePacker final : public AtlasPacker
{
public:
    SimplePacker(uint32_t count, uint32_t padding, uint32_t border);
    ~SimplePacker();

    bool compare(const cImage* a, const cImage* b) const override;

    void setSize(const sSize& size) override;
    bool add(const cImage* image) override;
    void makeAtlas(bool overlay) override;

    uint32_t getRectsCount() const override;
    const cImage* getImageByIndex(uint32_t idx) const override;
    const sRect& getRectByIndex(uint32_t idx) const override;

private:
    const sRect* checkRegion(const sRect& region) const;

private:
    struct sPiece
    {
        const cImage* image;
        sRect rc;
    };
    std::vector<sPiece> m_images;
};
