/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "types/bitmap.h"

struct sConfig;

class cTrim
{
public:
    virtual ~cTrim() = default;

    virtual bool trim(const char* name, const sBitmap& input);

    const sBitmap& getBitmap() const
    {
        return m_bitmap;
    }

protected:
    uint32_t findLeft(const sBitmap& input) const;
    uint32_t findRigth(const sBitmap& input) const;
    uint32_t findTop(const sBitmap& input) const;
    uint32_t findBottom(const sBitmap& input) const;

protected:
    sBitmap m_bitmap;
};



class cTrimRigthBottom final : public cTrim
{
public:
    cTrimRigthBottom(const sConfig& config);

    virtual bool trim(const char* name, const sBitmap& input) override;

private:
    const sConfig& m_config;
};
