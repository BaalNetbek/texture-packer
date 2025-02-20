/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "Atlas/AtlasPacker.h"
#include "Atlas/AtlasSize.h"
#include "Config.h"
#include "Image.h"
#include "ImageSaver.h"
#include "Trim.h"
#include "Types/Types.h"
#include "Utils.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <memory>
#include <string>
#include <vector>

using ImagesList = std::vector<cImage*>;

struct FileInfo
{
    uint32_t trimCount;
    std::string path;
};

using FilesList = std::vector<FileInfo>;

void showHelp(const char* name, const sConfig& config);
void printOversizeError(const sConfig& config, const sSize& atlasSize);
void addPath(uint32_t trimCount, const std::string& path, bool recurse, FilesList& filesList);
bool prepareSize(AtlasPacker* packer, const ImagesList& imagesList, const sSize& atlasSize);

int main(int argc, char* argv[])
{
    sConfig config;

    ::printf("Texture Packer v1.3.1.\n");
    ::printf("Copyright (c) 2017-2024 Andrey A. Ugolnik.\n\n");
    if (argc < 3)
    {
        showHelp(argv[0], config);
        return -1;
    }

    const char* outputAtlasName = nullptr;
    const char* outputResName = nullptr;
    const char* resPathPrefix = nullptr;
    FilesList filesList;

    uint32_t trimCount = 0;
    bool recurse = true;

    for (int i = 1; i < argc; i++)
    {
        const char* arg = argv[i];

        if (::strcmp(arg, "-o") == 0)
        {
            if (i + 1 < argc)
            {
                outputAtlasName = argv[++i];
            }
        }
        else if (::strcmp(arg, "-res") == 0)
        {
            if (i + 1 < argc)
            {
                outputResName = argv[++i];
            }
        }
        else if (::strcmp(arg, "-prefix") == 0)
        {
            if (i + 1 < argc)
            {
                resPathPrefix = argv[++i];
            }
        }
        else if (::strcmp(arg, "-b") == 0)
        {
            if (i + 1 < argc)
            {
                config.border = static_cast<uint32_t>(::atoi(argv[++i]));
            }
        }
        else if (::strcmp(arg, "-p") == 0)
        {
            if (i + 1 < argc)
            {
                config.padding = static_cast<uint32_t>(::atoi(argv[++i]));
            }
        }
        else if (::strcmp(arg, "-max") == 0)
        {
            if (i + 1 < argc)
            {
                config.maxTextureSize = static_cast<uint32_t>(::atoi(argv[++i]));
            }
        }
        else if (::strcmp(arg, "-tl") == 0)
        {
            if (i + 1 < argc)
            {
                trimCount = static_cast<uint32_t>(::atoi(argv[++i]));
            }
        }
        else if (::strcmp(arg, "-pot") == 0)
        {
            config.pot = true;
        }
        else if (::strcmp(arg, "-trim") == 0)
        {
            config.trim = true;
        }
        else if (::strcmp(arg, "-dupes") == 0)
        {
            config.alowDupes = true;
        }
        else if (::strcmp(arg, "-slow") == 0)
        {
            config.slowMethod = true;
        }
        else if (::strcmp(arg, "-dropext") == 0)
        {
            config.dropExt = true;
        }
        else if (::strcmp(arg, "-overlay") == 0)
        {
            config.overlay = true;
        }
        else if (::strcmp(arg, "-nr") == 0)
        {
            recurse = false;
        }
        else
        {
            auto dir = ::opendir(arg);
            if (dir != nullptr)
            {
                ::closedir(dir);

                std::string path = arg;
                if (path[path.length() - 1] == '/')
                {
                    path.pop_back();
                }
                addPath(trimCount, path, recurse, filesList);

                recurse = true;
            }
            else
            {
                if (cImage::IsImage(arg))
                {
                    filesList.push_back({ trimCount, arg });
                }
            }
        }
    }

    if (outputAtlasName == nullptr)
    {
        ::printf("No output name defined.\n");
        return -1;
    }

    ::printf("Border %u px.\n", config.border);
    ::printf("Padding %u px.\n", config.padding);
    ::printf("Overlay: %s.\n", isEnabled(config.overlay));
    ::printf("Allow dupes: %s.\n", isEnabled(config.alowDupes));
    ::printf("Trim sprites: %s.\n", isEnabled(config.trim));
    ::printf("Power of Two: %s.\n", isEnabled(config.pot));
    ::printf("Packing method: %s.\n", config.slowMethod ? "Slow" : "KD-Tree");
    ::printf("Drop extension: %s.\n", isEnabled(config.dropExt));
    ::printf("Max atlas size %u px.\n", config.maxTextureSize);
    if (resPathPrefix != nullptr)
    {
        ::printf("Resource path prefix: %s.\n", resPathPrefix);
    }
    ::printf("\n");

    const auto totalFiles = (uint32_t)filesList.size();

    // sort and remove dupes
    if (config.alowDupes == false)
    {
        std::sort(filesList.begin(), filesList.end(), [](const FileInfo& a, const FileInfo& b) {
            return a.path < b.path;
        });
        auto it = std::unique(filesList.begin(), filesList.end(), [](const FileInfo& a, const FileInfo& b) {
            return a.path == b.path;
        });
        filesList.resize(std::distance(filesList.begin(), it));
    }

    // load images
    auto startTime = getCurrentTime();
    std::unique_ptr<cTrim> trim(config.trim ? new cTrim() : nullptr);
    ImagesList imagesList;
    imagesList.reserve(filesList.size());

    cAtlasSize sizeCalculator(config);

    for (const auto& f : filesList)
    {
        std::unique_ptr<cImage> image(new cImage());
        if (image->load(f.path.c_str(), f.trimCount, trim.get()) == true)
        {
            auto& bmp = image->getBitmap();
            auto& size = bmp.getSize();
            sizeCalculator.addRect(size);

            imagesList.push_back(image.release());
        }
        else
        {
            ::printf("(WW) Image '%s' not loaded.\n", f.path.c_str());
        }
    }

    auto ms = (getCurrentTime() - startTime) * 0.001f;
    ::printf("Loaded %u (%u) images in %g ms.\n", (uint32_t)imagesList.size(), totalFiles, ms);

    if (imagesList.size() > 0)
    {
        auto packer = AtlasPacker::create(imagesList.size(), config);

        std::stable_sort(imagesList.begin(), imagesList.end(), [&packer](const cImage* a, const cImage* b) -> bool {
            return packer->compare(a, b);
        });

        auto atlasSize = sizeCalculator.calcSize();
        if (sizeCalculator.isGood(atlasSize) == false)
        {
            printOversizeError(config, atlasSize);
            return -1;
        }

        ::printf("Packing:\n");
        ::printf(" - trying %u x %u.\n", atlasSize.width, atlasSize.height);
        ::fflush(nullptr);

        startTime = getCurrentTime();

        bool done = false;
        do
        {
            if (prepareSize(packer.get(), imagesList, atlasSize) == false)
            {
                atlasSize = sizeCalculator.nextSize(atlasSize, 8u);
                if (sizeCalculator.isGood(atlasSize) == false)
                {
                    printOversizeError(config, atlasSize);
                    return -1;
                }

                ::printf(" - trying %u x %u.\n", atlasSize.width, atlasSize.height);
                ::fflush(nullptr);
            }
            else
            {
                packer->buildAtlas();
                auto& atlas = packer->getBitmap();

                cImageSaver saver(atlas, outputAtlasName);

                // write texture
                if (saver.save() == true)
                {
                    outputAtlasName = saver.getAtlasName();

                    // write resource file
                    if (outputResName != nullptr)
                    {
                        std::string atlasName = resPathPrefix != nullptr
                            ? resPathPrefix
                            : "";
                        atlasName += outputAtlasName;

                        packer->generateResFile(outputResName, atlasName.c_str());
                    }

                    auto spritesArea = sizeCalculator.getArea();
                    auto atlasArea = atlasSize.width * atlasSize.height;
                    auto percent = static_cast<uint32_t>(100.0f * spritesArea / atlasArea);

                    ::printf("Atlas '%s' (%u x %u, fill: %u%%) has been created",
                             outputAtlasName,
                             atlasSize.width,
                             atlasSize.height,
                             percent);
                }
                else
                {
                    ::printf("Error writting atlas '%s' (%u x %u)", outputAtlasName,
                             atlasSize.width,
                             atlasSize.height);
                }

                auto ms = (getCurrentTime() - startTime) * 0.001f;
                ::printf(" in %g ms.\n", ms);
                ::fflush(nullptr);

                done = true;
            }
        } while (done == false);

        for (auto img : imagesList)
        {
            delete img;
        }
    }

    return 0;
}

void showHelp(const char* name, const sConfig& config)
{
    ::printf("Usage:\n");
    auto p = ::strrchr(name, '/');
    ::printf("  %s INPUT_IMAGE [INPUT_IMAGE] -o ATLAS\n\n", p ? p + 1 : name);
    ::printf("  INPUT_IMAGE        input image name or directory separated by space\n");
    ::printf("  -o ATLAS           output atlas name (default PNG)\n");
    ::printf("  -res DESC_TEXTURE  output atlas description as XML\n");
    ::printf("  -prefix STRING     add prefix to texture path\n");
    ::printf("  -pot               make power of two atlas (default %s)\n", isEnabled(config.pot));
    ::printf("  -nr                don't recurse in next directory\n");
    ::printf("  -tl count          trim left sprite's id by count (default 0)\n");
    ::printf("  -trim              trim sprites (default %s)\n", isEnabled(config.trim));
    ::printf("  -overlay           overlay sprites (default %s)\n", isEnabled(config.overlay));
    ::printf("  -dupes             allow dupes (default %s)\n", isEnabled(config.alowDupes));
    ::printf("  -slow              use slow method instead kd-tree (default %s)\n", isEnabled(config.slowMethod));
    ::printf("  -b size            add border around sprites (default %u px)\n", config.border);
    ::printf("  -p size            add padding between sprites (default %u px)\n", config.padding);
    ::printf("  -dropext           drop file extension from sprite id (default %s)\n", isEnabled(config.dropExt));
    ::printf("  -max size          max atlas size (default %u px)\n", config.maxTextureSize);
}

void printOversizeError(const sConfig& config, const sSize& atlasSize)
{
    ::printf("\n");
    ::printf("Desired texture size %u x %u, but maximum %u x %u.\n",
             atlasSize.width,
             atlasSize.height,
             config.maxTextureSize,
             config.maxTextureSize);
    ::fflush(nullptr);
}

int DirectoryFilter(const dirent* p)
{
    // skip . and ..
#define DOT_OR_DOTDOT(base) (base[0] == '.' && (base[1] == '\0' || (base[1] == '.' && base[2] == '\0')))
    return DOT_OR_DOTDOT(p->d_name) ? 0 : 1;
}

void addPath(uint32_t trimCount, const std::string& root, bool recurse, FilesList& filesList)
{
    dirent** namelist;
    int n = ::scandir(root.c_str(), &namelist, DirectoryFilter, alphasort);
    if (n >= 0)
    {
        while (n--)
        {
            std::string path(root);
            path += "/";
            path += namelist[n]->d_name;

            // skip non non readable files/dirs
            auto dir = opendir(path.c_str());
            if (dir != nullptr)
            {
                ::closedir(dir);
                if (recurse)
                {
                    addPath(trimCount, path, recurse, filesList);
                }
            }
            else if (cImage::IsImage(path.c_str()))
            {
                filesList.push_back({ trimCount, path });
            }
            ::free(namelist[n]);
        }
        ::free(namelist);
    }
}

bool prepareSize(AtlasPacker* packer, const ImagesList& imagesList, const sSize& atlasSize)
{
    packer->setSize(atlasSize);
    for (size_t i = 0, size = imagesList.size(); i < size; i++)
    {
        const auto& img = imagesList[i];
        if (packer->add(img) == false)
        {
            return false;
        }
    }

    return true;
}
