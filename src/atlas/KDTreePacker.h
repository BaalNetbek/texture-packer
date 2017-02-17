/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "AtlasPacker.h"

class cKDNode;

class KDTreePacker final : public AtlasPacker
{
public:
    KDTreePacker(uint32_t padding, uint32_t border);
    ~KDTreePacker();

    bool compare(const cImage* a, const cImage* b) const override;

    void setSize(const sSize& size) override;
    bool add(const cImage* image) override;
    void makeAtlas(bool overlay) override;

    uint32_t getRectsCount() const override;
    const cImage* getImageByIndex(uint32_t idx) const override;
    const sRect& getRectByIndex(uint32_t idx) const override;

private:
    cKDNode* m_root;

    struct sPiece
    {
        const cImage* image;
        cKDNode* node;
    };
    std::vector<sPiece> m_nodes;
};
