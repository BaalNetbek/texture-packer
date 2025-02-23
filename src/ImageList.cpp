/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "ImageList.h"
#include "Atlas/AtlasPacker.h"
#include "Config.h"
#include "File.h"
#include "Image.h"
#include "ImageSaver.h"
#include "Log.h"
#include "Trim.h"
#include "Utils.h"

#include <sstream>

cImageList::cImageList(const sConfig& config, uint32_t reserve)
    : m_config(config)
    , m_size(config)
    , m_trim(config.trim
                 ? new cTrim()
                 : nullptr)
{
    m_images.reserve(reserve);
}

cImageList::~cImageList()
{
    for (auto img : m_images)
    {
        delete img;
    }

    delete m_trim;
}

const cImage* cImageList::loadImage(const std::string& path, uint32_t trimCount)
{
    if (cImage::IsImage(path.c_str()))
    {
        std::unique_ptr<cImage> image(new cImage());

        if (image->load(path.c_str(), trimCount, m_trim) == true)
        {
            auto& bmp = image->getBitmap();
            auto& size = bmp.getSize();
            m_size.addRect(size);

            m_images.push_back(image.release());

            return m_images.back();
        }
    }

    return nullptr;
}

bool cImageList::doPacking(const char* desiredAtlasName, const char* outputResName,
                           const char* resPathPrefix, sSize& atlasSize)
{
    if (m_images.size() == 0)
    {
        return true;
    }

    atlasSize = m_size.calcSize();
    if (m_size.isGood(atlasSize) == false)
    {
        return false;
    }

    auto startTime = getCurrentTime();

    AtlasPacker::sort(m_images, m_config);
    auto packer = AtlasPacker::create(m_images.size(), m_config);

    cLog::Info("Packing:");
    cLog::Info(" - trying {} x {}.", atlasSize.width, atlasSize.height);

    bool done = false;
    do
    {
        if (prepareSize(packer.get(), atlasSize) == false)
        {
            atlasSize = m_size.nextSize(atlasSize, 8u);
            if (m_size.isGood(atlasSize) == false)
            {
                return false;
            }

            cLog::Info(" - trying {} x {}.", atlasSize.width, atlasSize.height);
        }
        else
        {
            packer->buildAtlas();
            auto& atlas = packer->getBitmap();

            cImageSaver saver(atlas, desiredAtlasName);

            // write texture
            if (saver.save() == true)
            {
                auto outputAtlasName = saver.getAtlasName();

                // write resource file
                if (outputResName != nullptr)
                {
                    std::string atlasName = resPathPrefix != nullptr
                        ? resPathPrefix
                        : "";
                    atlasName += outputAtlasName;

                    cFile file;
                    if (file.open(outputResName, "w"))
                    {
                        writeHeader(file);

                        packer->generateResFile(file, atlasName);

                        writeFooter(file);
                    }
                }

                auto spritesArea = m_size.getArea();
                auto atlasArea = atlasSize.width * atlasSize.height;
                auto percent = static_cast<uint32_t>(100.0f * spritesArea / atlasArea);

                cLog::Info("Atlas '{}' ({} x {}, fill: {}%) was created in {:.2f} ms.",
                           outputAtlasName.c_str(),
                           atlasSize.width, atlasSize.height,
                           percent,
                           (getCurrentTime() - startTime) * 0.001f);
            }
            else
            {
                cLog::Error("Error writing atlas '{}' ({} x {})",
                            desiredAtlasName,
                            atlasSize.width, atlasSize.height);
            }

            done = true;
        }
    } while (done == false);

    return true;
}

bool cImageList::prepareSize(AtlasPacker* packer, const sSize& atlasSize)
{
    packer->setSize(atlasSize);
    for (auto img : m_images)
    {
        if (packer->add(img) == false)
        {
            return false;
        }
    }

    return true;
}

bool cImageList::prepareAtlas(AtlasPacker* packer, sSize& atlasSize)
{
    do
    {
        auto result = prepareSize(packer, atlasSize);
        if (result)
        {
            return true;
        }
        else
        {
            auto desiredSize = m_size.nextSize(atlasSize, 8u);
            if (m_size.isGood(desiredSize) == false)
            {
                return false;
            }

            atlasSize = desiredSize;
            cLog::Info(" - trying {} x {}.", atlasSize.width, atlasSize.height);
        }
    } while (true);
}

void cImageList::writeHeader(cFile& file)
{
    std::stringstream out;
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<atlas>\n";
    file.write((void*)out.str().c_str(), out.str().length());
}

void cImageList::writeFooter(cFile& file)
{
    std::stringstream out;
    out << "</atlas>\n";
    file.write((void*)out.str().c_str(), out.str().length());
}
