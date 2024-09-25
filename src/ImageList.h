/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "Atlas/AtlasSize.h"

#include <string>
#include <vector>

class AtlasPacker;
class cFile;
class cImage;
class cTrim;
struct sConfig;

class cImageList final
{
public:
    cImageList(const sConfig& config, uint32_t reserve);
    ~cImageList();

    const cImage* loadImage(const std::string& path, uint32_t trimCount);

    bool doPacking(const char* desiredAtlasName, const char* outputResName,
                   const char* resPathPrefix, sSize& atlasSize);

    using List = std::vector<cImage*>;

    const List& getList() const
    {
        return m_images;
    }

private:
    bool prepareSize(AtlasPacker* packer, const sSize& atlasSize);
    void writeHeader(cFile& file);
    void writeFooter(cFile& file);

private:
    const sConfig& m_config;
    cAtlasSize m_size;
    cTrim* m_trim;

private:
    List m_images;
};
