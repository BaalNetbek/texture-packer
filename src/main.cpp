/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "Config.h"
#include "FileList.h"
#include "ImageList.h"
#include "Utils.h"

#include <cstdlib>
#include <cstring>
#include <string>

void showHelp(const char* name, const sConfig& config);

int main(int argc, char* argv[])
{
    sConfig config;

    ::printf("Texture Packer v1.3.2.\n");
    ::printf("Copyright (c) 2017-2024 Andrey A. Ugolnik.\n\n");
    if (argc < 3)
    {
        showHelp(argv[0], config);
        return -1;
    }

    const char* outputAtlasName = nullptr;
    const char* outputResName = nullptr;
    const char* resPathPrefix = nullptr;

    cFileList fileList;

    uint32_t trimCount = 0u;
    auto recurse = true;

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
            fileList.addPath(trimCount, arg, recurse);
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

    auto startTime = getCurrentTime();

    // sort and remove dupes
    if (config.alowDupes == false)
    {
        fileList.removeDupes();
    }

    // load images
    auto& files = fileList.getList();
    cImageList imageList(config, files.size());

    for (const auto& f : files)
    {
        auto image = imageList.loadImage(f.path, f.trimCount);
        if (image == nullptr)
        {
            ::printf("(WW) File '%s' not loaded.\n", f.path.c_str());
        }
    }

    auto& images = imageList.getList();

    ::printf("Loaded %u (%u) images in %g ms.\n",
             static_cast<uint32_t>(images.size()),
             static_cast<uint32_t>(files.size()),
             (getCurrentTime() - startTime) * 0.001f);

    // packing
    sSize atlasSize;
    if (imageList.doPacking(outputAtlasName, outputResName, resPathPrefix, atlasSize) == false)
    {
        ::printf("\n");
        ::printf("Desired texture size %u x %u, but maximum %u x %u.\n",
                 atlasSize.width,
                 atlasSize.height,
                 config.maxTextureSize,
                 config.maxTextureSize);
        ::fflush(nullptr);

        return -1;
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
