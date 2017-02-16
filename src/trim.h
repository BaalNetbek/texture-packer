/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "bitmap.h"

struct sConfig;

class cTrim
{
public:
    virtual ~cTrim() = default;

    virtual bool trim(const char* name, const Bitmap& input);

    const Bitmap& getBitmap() const
    {
        return m_bitmap;
    }

protected:
    uint32_t findLeft(const Bitmap& bitmap) const;
    uint32_t findRigth(const Bitmap& bitmap) const;
    uint32_t findTop(const Bitmap& bitmap) const;
    uint32_t findBottom(const Bitmap& bitmap) const;

protected:
    Bitmap m_bitmap;
};



class cTrimBorder final : public cTrim
{
public:
    cTrimBorder(const sConfig& config);

    virtual bool trim(const char* name, const Bitmap& input) override;

private:
    const sConfig& m_config;
};
