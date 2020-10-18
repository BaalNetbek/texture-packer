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

    virtual bool trim(const char* path, const cBitmap& input);

    const cBitmap& getBitmap() const
    {
        return m_bitmap;
    }

protected:
    uint32_t findLeft(const cBitmap& input) const;
    uint32_t findRigth(const cBitmap& input) const;
    uint32_t findTop(const cBitmap& input) const;
    uint32_t findBottom(const cBitmap& input) const;

protected:
    cBitmap m_bitmap;
};



class cTrimRigthBottom final : public cTrim
{
public:
    cTrimRigthBottom(const sConfig& config);

    virtual bool trim(const char* path, const cBitmap& input) override;

private:
    const sConfig& m_config;
};
