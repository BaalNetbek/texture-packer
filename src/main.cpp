/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "image.h"
#include "imagesaver.h"
#include "packer.h"
#include "trim.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <dirent.h>
#include <sys/time.h>

void showHelp(const char* name);
unsigned nextPot(unsigned i);
uint64_t getCurrentTime();

typedef std::vector<std::string> FilesList;
void addPath(const std::string& path, FilesList& filesList);

int main(int argc, char* argv[])
{
    printf("Texture Packer v1.0.2.\n");
    printf("Copyright (c) 2017 Andrey A. Ugolnik.\n\n");
    if (argc < 3)
    {
        showHelp(argv[0]);
        return -1;
    }

    const char* outputAtlasName = nullptr;
    const char* outputResName = nullptr;
    FilesList filesList;

    unsigned maxTextureSize = 4096;
    unsigned border = 1;
    bool pot = false;
    bool trimInput = false;

    for (int i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], "-o", 2) == 0)
        {
            if (i + 1 < argc)
            {
                outputAtlasName = argv[++i];
            }
        }
        else if (strncmp(argv[i], "-res", 4) == 0)
        {
            if (i + 1 < argc)
            {
                outputResName = argv[++i];
            }
        }
        else if (strncmp(argv[i], "-b", 2) == 0)
        {
            if (i + 1 < argc)
            {
                border = static_cast<unsigned>(atoi(argv[++i]));
            }
        }
        else if (strncmp(argv[i], "-max", 4) == 0)
        {
            if (i + 1 < argc)
            {
                maxTextureSize = static_cast<unsigned>(atoi(argv[++i]));
            }
        }
        else if (strncmp(argv[i], "-pot", 4) == 0)
        {
            pot = true;
        }
        else if (strncmp(argv[i], "-trim", 5) == 0)
        {
            trimInput = true;
        }
        else
        {
            std::string path = argv[i];
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

    printf("Border %u px.\n", border);
    printf("Trim images: %s.\n", trimInput ? "true" : "false");
    printf("Power of Two: %s.\n", pot ? "true" : "false");
    printf("Max texture size %u px.\n", maxTextureSize);

    auto startTime = getCurrentTime();
    std::unique_ptr<cTrim> trim(trimInput ? new cTrim() : nullptr);
    std::vector<cImage*> imagesList;
    for (const auto& path : filesList)
    {
        auto image = std::make_unique<cImage>();
        if (image->load(path.c_str(), trim.get()) == true)
        {
            imagesList.push_back(image.release());
        }
    }
    auto sec = (getCurrentTime() - startTime) * 0.000001f;
    printf("Loaded %u images in %g sec.\n", (unsigned)imagesList.size(), sec);

    if (imagesList.size() > 0)
    {
        std::sort(imagesList.begin(), imagesList.end(), [](const cImage * a, const cImage * b)
        {
            auto& bmpa = a->getBitmap();
            auto& bmpb = b->getBitmap();
            return bmpa.width * bmpa.height > bmpb.width * bmpb.height;
        });

        printf("Packing");
        cPacker packer(imagesList.size(), border);

        unsigned area = 0;
        for (auto img : imagesList)
        {
            auto& bmp = img->getBitmap();
            area += (bmp.width + border * 2) * (bmp.height + border * 2);
        }

        auto width = static_cast<unsigned>(sqrt(area));
        auto height = static_cast<unsigned>(sqrt(area));
        if (pot)
        {
            width = nextPot(width);
            height = nextPot(area / width);
        }

        // printf("Start from %d x %d.\n", width, height);
        startTime = getCurrentTime();

        bool done = true;
        do
        {
            done = true;
            packer.setSize(width, height);
            for (auto img : imagesList)
            {
                if (packer.add(img) == false)
                {
                    printf(".");
                    fflush(nullptr);

                    if (pot)
                    {
                        if (width > height)
                        {
                            height <<= 1;
                        }
                        else
                        {
                            width <<= 1;
                        }
                    }
                    else
                    {
                        // expand texture
                        if ((height & (height - 1)) != 0)
                        {
                            height += 2;
                        }
                        else
                        {
                            width += 2;
                            if ((width & (width - 1)) == 0)
                            {
                                height += 2;
                            }
                        }
                    }
                    done = false;
                    break;
                }
            }
        }
        while (done == false && width <= maxTextureSize && height <= maxTextureSize);

        auto sec = (getCurrentTime() - startTime) * 0.000001f;
        printf(" in %g sec.\n", sec);

        if (width > maxTextureSize || height > maxTextureSize)
        {
            printf("Resulting texture too big.\n");
        }

        packer.fillTexture();

        // write texture
        cImageSaver saver(packer.getBitmap());
        if (saver.save(outputAtlasName) == true)
        {
            // write resource file
            if (outputResName != nullptr)
            {
                packer.generateResFile(outputResName, outputAtlasName);
            }

            printf("Atlas '%s' %u x %u has been created.\n", outputAtlasName, width, height);
        }

        for (auto img : imagesList)
        {
            delete img;
        }
    }

    return 0;
}

void showHelp(const char* name)
{
    printf("Usage:\n");
    const char* p = strrchr(name, '/');
    printf("  %s INPUT_IMAGE [INPUT_IMAGE] -o ATLAS\n\n", p ? p + 1 : name);
    printf("  INPUT_IMAGE        input image name or directory separated by space\n");
    printf("  -o ATLAS           output atlas name (default PNG)\n");
    printf("  -res DESC_TEXTURE  output atlas description as XML\n");
    printf("  -pot               make power of two texture\n");
    printf("  -trim              trim input images\n");
    printf("  -b size            add border around image (default 1 px)\n");
    printf("  -max size          max texture size\n");
}

unsigned nextPot(unsigned i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;
    i++;
    return i;
}

uint64_t getCurrentTime()
{
    timeval t;
    ::gettimeofday(&t, 0);

    return (uint64_t)(t.tv_sec * 1000000 + t.tv_usec);
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
