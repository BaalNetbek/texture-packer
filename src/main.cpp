/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "atlas/AtlasPacker.h"
#include "config.h"
#include "image.h"
#include "imagesaver.h"
#include "trim.h"
#include "types/size.h"
#include "utils.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <memory>
#include <string>
#include <vector>

void showHelp(const char* name, const sConfig& config);

struct FileInfo
{
    uint32_t trimCount;
    std::string path;
};

typedef std::vector<FileInfo> FilesList;
void addPath(uint32_t trimCount, const std::string& path, bool recurse, FilesList& filesList);

sSize calcSize(uint32_t area, const sSize& maxRectSize, const sConfig& config);

int main(int argc, char* argv[])
{
    sConfig config;

    ::printf("Texture Packer v1.3.0.\n");
    ::printf("Copyright (c) 2017-2020 Andrey A. Ugolnik.\n\n");
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
        // else if (::strcmp(arg, "-max") == 0)
        // {
        // if (i + 1 < argc)
        // {
        // config.maxTextureSize = static_cast<uint32_t>(::atoi(argv[++i]));
        // }
        // }
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
    // ::printf("Max atlas size %u px.\n", config.maxTextureSize);
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

    uint32_t area = 0;
    sSize maxRectSize;

    // load images
    auto startTime = getCurrentTime();
    std::unique_ptr<cTrim> trim(config.trim ? new cTrim() : nullptr);
    std::vector<cImage*> imagesList;
    imagesList.reserve(filesList.size());

    for (const auto& f : filesList)
    {
        std::unique_ptr<cImage> image(new cImage());
        if (image->load(f.path.c_str(), f.trimCount, trim.get()) == true)
        {
            // printf("(II) path: %s loaded.\n", f.path.c_str());
            auto& bmp = image->getBitmap();
            maxRectSize.width = std::max<uint32_t>(maxRectSize.width, bmp.getWidth() + config.padding);
            maxRectSize.height = std::max<uint32_t>(maxRectSize.height, bmp.getHeight() + config.padding);
            area += (bmp.getHeight() + config.padding) * (bmp.getHeight() + config.padding);

            auto ptr = image.release();
            // printf("(II) ptr: 0x%p , path: %s loaded\n", static_cast<const void*>(ptr), f.path.c_str());
            imagesList.push_back(ptr);
        }
        else
        {
            printf("(EE) path: %s not loaded.\n", f.path.c_str());
        }
    }

    auto sec = (getCurrentTime() - startTime) * 0.000001f;
    ::printf("Loaded %u (%u) images in %g sec.\n", (uint32_t)imagesList.size(), totalFiles, sec);

    if (imagesList.size() > 0)
    {
        std::unique_ptr<AtlasPacker> packer(AtlasPacker::create(imagesList.size(), config));

        int i = 0;
        std::stable_sort(imagesList.begin(), imagesList.end(), [&packer,&i](const cImage* a, const cImage* b) -> bool {
            return packer->compare(a, b);
        });

        auto texSize = calcSize(area, maxRectSize, config);
        // ::printf("Start from %u x %u.\n", texSize.width, texSize.height);

        startTime = getCurrentTime();

        ::printf("Packing ");
        ::fflush(nullptr);
        bool done = true;
        do
        {
            done = true;
            packer->setSize(texSize);
            for (size_t i = 0, size = imagesList.size(); i < size; i++)
            {
                const auto& img = imagesList[i];

                if (packer->add(img) == false)
                {
                    done = false;

                    for (; i < size; i++)
                    {
                        const auto& bmp = imagesList[i]->getBitmap();
                        auto s = (bmp.getWidth() + config.padding) * (bmp.getHeight() + config.padding);
                        area += s;
                    }

                    texSize = calcSize(area, maxRectSize, config);

                    // ::printf(" new texture size %u x %u.\n", texSize.width, texSize.height);
                    ::printf(".");
                    ::fflush(nullptr);
                    break;
                }
            }

            if (done)
            {
                auto sec = (getCurrentTime() - startTime) * 0.000001f;
                ::printf(" in %g sec.\n", sec);
                ::fflush(nullptr);

                packer->buildAtlas();

                auto& atlas = packer->getBitmap();
                // write texture
                cImageSaver saver(packer->getBitmap(), outputAtlasName);
                if (saver.save() == true)
                {
                    outputAtlasName = saver.getAtlasName();

                    // write resource file
                    if (outputResName != nullptr)
                    {
                        std::string atlasName = resPathPrefix != nullptr ? resPathPrefix : "";
                        atlasName += outputAtlasName;

                        packer->generateResFile(outputResName, atlasName.c_str());
                    }

                    ::printf("Atlas '%s' %u x %u (%s px) has been created.\n",
                             outputAtlasName,
                             atlas.getWidth(),
                             atlas.getHeight(),
                             formatNum(atlas.getWidth() * atlas.getHeight()));
                }
            }
        } while (done == false); // && texSize.width <= config.maxTextureSize && texSize.height <= config.maxTextureSize);

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
    // ::printf("  -max size          max atlas size (default %u px)\n", config.maxTextureSize);
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

sSize calcSize(uint32_t area, const sSize& maxRectSize, const sConfig& config)
{
    auto sq = static_cast<uint32_t>(sqrt(area));
    auto width = std::max<uint32_t>(sq, maxRectSize.width) + config.border * 2;
    auto height = std::max<uint32_t>(sq, maxRectSize.height) + config.border * 2;

    width = fixSize(width, config.pot);
    height = fixSize(area / width, config.pot);

    return { width, height };
}
