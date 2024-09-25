/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <vector>

class AtlasPacker;
class cImage;
class cTrim;
struct sConfig;

class cImageList final
{
public:
    cImageList(const sConfig& config, uint32_t reserve);
    ~cImageList();

    const cImage* loadImage(const std::string& path, uint32_t trimCount);

    void sort(AtlasPacker* packer);

    using List = std::vector<cImage*>;

    const List& getList() const;

private:
    cTrim* m_trim;

private:
    List m_images;
};
