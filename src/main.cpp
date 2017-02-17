/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "config.h"
#include "image.h"
#include "imagesaver.h"
#include "packer.h"
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

typedef std::vector<std::string> FilesList;
void addPath(const std::string& path, FilesList& filesList);

sSize calcSize(uint32_t area, const sSize& maxRectSize, const sConfig& config);

int main(int argc, char* argv[])
{
    sConfig config;

    printf("Texture Packer v1.1.5.\n");
    printf("Copyright (c) 2017 Andrey A. Ugolnik.\n\n");
    if (argc < 3)
    {
        showHelp(argv[0], config);
        return -1;
    }

    const char* outputAtlasName = nullptr;
    const char* outputResName = nullptr;
    FilesList filesList;

    for (int i = 1; i < argc; i++)
    {
        const char* arg = argv[i];

        if (strcmp(arg, "-o") == 0)
        {
            if (i + 1 < argc)
            {
                outputAtlasName = argv[++i];
            }
        }
        else if (strcmp(arg, "-res") == 0)
        {
            if (i + 1 < argc)
            {
                outputResName = argv[++i];
            }
        }
        else if (strcmp(arg, "-b") == 0)
        {
            if (i + 1 < argc)
            {
                config.border = static_cast<uint32_t>(atoi(argv[++i]));
            }
        }
        else if (strcmp(arg, "-p") == 0)
        {
            if (i + 1 < argc)
            {
                config.padding = static_cast<uint32_t>(atoi(argv[++i]));
            }
        }
        // else if (strcmp(arg, "-max") == 0)
        // {
        // if (i + 1 < argc)
        // {
        // config.maxTextureSize = static_cast<uint32_t>(atoi(argv[++i]));
        // }
        // }
        else if (strcmp(arg, "-pot") == 0)
        {
            config.pot = true;
        }
        else if (strcmp(arg, "-trim") == 0)
        {
            config.trim = true;
        }
        else if (strcmp(arg, "-dupes") == 0)
        {
            config.alowDupes = true;
        }
        else if (strcmp(arg, "-slow") == 0)
        {
            config.slowMethod = true;
        }
        else if (strcmp(arg, "-overlay") == 0)
        {
            config.overlay = true;
        }
        else
        {
            std::string path = arg;
            if (path[path.length() - 1] == '/')
            {
                path = path.substr(0, path.length() - 1);
            }
            addPath(path, filesList);
        }
    }

    if (outputAtlasName == nullptr)
    {
        printf("No output name defined.\n");
        return -1;
    }

    printf("Border %u px.\n", config.border);
    printf("Padding %u px.\n", config.padding);
    printf("Overlay: %s.\n", isEnabled(config.overlay));
    printf("Allow dupes: %s.\n", isEnabled(config.alowDupes));
    printf("Trim sprites: %s.\n", isEnabled(config.trim));
    printf("Power of Two: %s.\n", isEnabled(config.pot));
    printf("Packing method: %s.\n", config.slowMethod ? "Slow" : "KD-Tree");
    // printf("Max atlas size %u px.\n", config.maxTextureSize);
    printf("\n");

    const auto totalFiles = (uint32_t)filesList.size();

    // sort and remove dupes
    if (config.alowDupes == false)
    {
        std::sort(filesList.begin(), filesList.end());
        auto it = std::unique(filesList.begin(), filesList.end());
        filesList.resize(std::distance(filesList.begin(), it));
    }

    uint32_t area = 0;
    sSize maxRectSize;

    // load images
    auto startTime = getCurrentTime();
    std::unique_ptr<cTrim> trim(config.trim ? new cTrim() : nullptr);
    std::vector<cImage*> imagesList;
    for (const auto& path : filesList)
    {
        std::unique_ptr<cImage> image(new cImage());
        if (image->load(path.c_str(), trim.get()) == true)
        {
            auto& bmp = image->getBitmap();
            maxRectSize.width = std::max<uint32_t>(maxRectSize.width, bmp.width + config.padding);
            maxRectSize.height = std::max<uint32_t>(maxRectSize.height, bmp.height + config.padding);
            area += (bmp.width + config.padding) * (bmp.height + config.padding);

            imagesList.push_back(image.release());
        }
    }
    auto sec = (getCurrentTime() - startTime) * 0.000001f;
    printf("Loaded %u (%u) images in %g sec.\n", (uint32_t)imagesList.size(), totalFiles, sec);

    if (imagesList.size() > 0)
    {
        cPacker packer(imagesList.size(), config);

        std::sort(imagesList.begin(), imagesList.end(), [&packer](const cImage * a, const cImage * b)
        {
            return packer.compare(a, b);
        });

        auto texSize = calcSize(area, maxRectSize, config);
        // printf("Start from %u x %u.\n", texSize.width, texSize.height);

        startTime = getCurrentTime();

        printf("Packing ");
        fflush(nullptr);
        bool done = true;
        do
        {
            done = true;
            packer.setSize(texSize);
            for (size_t i = 0, size = imagesList.size(); i < size; i++)
            {
                const auto& img = imagesList[i];

                if (packer.add(img) == false)
                {
                    done = false;

                    for (; i < size; i++)
                    {
                        const auto& bmp = imagesList[i]->getBitmap();
                        auto s = (bmp.width + config.padding) * (bmp.height + config.padding);
                        area += s;
                    }

                    texSize = calcSize(area, maxRectSize, config);

                    // printf(" new texture size %u x %u.\n", texSize.width, texSize.height);
                    printf(".");
                    fflush(nullptr);
                    break;
                }
            }

            if (done)
            {
                auto sec = (getCurrentTime() - startTime) * 0.000001f;
                printf(" in %g sec.\n", sec);
                fflush(nullptr);

                packer.buildAtlas();

                auto& atlas = packer.getBitmap();
                // write texture
                cImageSaver saver(packer.getBitmap(), outputAtlasName);
                if (saver.save() == true)
                {
                    outputAtlasName = saver.getAtlasName();

                    // write resource file
                    if (outputResName != nullptr)
                    {
                        packer.generateResFile(outputResName, outputAtlasName);
                    }

                    printf("Atlas '%s' %u x %u (%s px) has been created.\n", outputAtlasName, atlas.width, atlas.height, formatNum(atlas.width * atlas.height));
                }
            }
        }
        while (done == false);// && texSize.width <= config.maxTextureSize && texSize.height <= config.maxTextureSize);

        for (auto img : imagesList)
        {
            delete img;
        }
    }

    return 0;
}

void showHelp(const char* name, const sConfig& config)
{
    printf("Usage:\n");
    const char* p = strrchr(name, '/');
    printf("  %s INPUT_IMAGE [INPUT_IMAGE] -o ATLAS\n\n", p ? p + 1 : name);
    printf("  INPUT_IMAGE        input image name or directory separated by space\n");
    printf("  -o ATLAS           output atlas name (default PNG)\n");
    printf("  -res DESC_TEXTURE  output atlas description as XML\n");
    printf("  -pot               make power of two atlas (default %s)\n", isEnabled(config.pot));
    printf("  -trim              trim sprites (default %s)\n", isEnabled(config.trim));
    printf("  -overlay           overlay sprites (default %s)\n", isEnabled(config.overlay));
    printf("  -dupes             allow dupes (default %s)\n", isEnabled(config.alowDupes));
    printf("  -slow              use slow method instead kd-tree (default %s)\n", isEnabled(config.slowMethod));
    printf("  -b size            add border around sprites (default %u px)\n", config.border);
    printf("  -p size            add padding between sprites (default %u px)\n", config.padding);
    // printf("  -max size          max atlas size (default %u px)\n", config.maxTextureSize);
}

int DirectoryFilter(const dirent* p)
{
    // skip . and ..
#define DOT_OR_DOTDOT(base) (base[0] == '.' && (base[1] == '\0' || (base[1] == '.' && base[2] == '\0')))
    return DOT_OR_DOTDOT(p->d_name) ? 0 : 1;
}

void addPath(const std::string& root, FilesList& filesList)
{
    dirent** namelist;
    int n = scandir(root.c_str(), &namelist, DirectoryFilter, alphasort);
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
                closedir(dir);
                // if (m_recursive == true)
                {
                    addPath(path, filesList);
                }
            }
            else if (cImage::IsImage(path.c_str()))
            {
                filesList.push_back(path);
            }
            free(namelist[n]);
        }
        free(namelist);
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
