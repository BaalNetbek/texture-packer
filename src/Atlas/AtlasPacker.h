/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "Types/Bitmap.h"

#include <memory>

class cFile;
class cImage;
struct sConfig;
struct sRect;
struct sSize;

class AtlasPacker
{
public:
    static std::unique_ptr<AtlasPacker> create(uint32_t count, const sConfig& config);

public:
    AtlasPacker(const sConfig& config);
    virtual ~AtlasPacker();

    virtual bool compare(const cImage* a, const cImage* b) const = 0;

    virtual void setSize(const sSize& size) = 0;
    virtual bool add(const cImage* image) = 0;
    virtual void makeAtlas(bool overlay) = 0;

    const cBitmap& getBitmap() const
    {
        return m_atlas;
    }

    cBitmap& getBitmap()
    {
        return m_atlas;
    }

    virtual uint32_t getRectsCount() const = 0;
    virtual const cImage* getImageByIndex(uint32_t idx) const = 0;
    virtual const sRect& getRectByIndex(uint32_t idx) const = 0;

    void buildAtlas();

    bool generateResFile(cFile& file, const std::string& atlasName);

protected:
    void copyBitmap(const sRect& rc, const cImage* image, bool overlay);

protected:
    const sConfig& m_config;

protected:
    cBitmap m_atlas;
};
