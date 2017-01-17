/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "image.h"
#include "file.h"
#include "trim.h"

#include <cstring>
#include <png.h>

bool cImage::IsImage(const char* name)
{
    cFile file;
    if (file.open(name))
    {
        png_byte header[8]; // 8 is the maximum size that can be checked
        return file.read(header, sizeof(header))== sizeof(header)
            && png_sig_cmp(header, 0, sizeof(header)) == 0;
    }

    return false;
}

bool cImage::load(const char* name, cTrim* trim)
{
    m_name = name;

    cFile file;
    if (file.open(name) == false)
    {
        return false;
    }

    png_byte header[8]; // 8 is the maximum size that can be checked
    auto size = file.read(header, sizeof(header));
    if (size != sizeof(header) || png_sig_cmp(header, 0, sizeof(header)) != 0)
    {
        printf("File '%s' is not recognized as a PNG file.\n", name);
        return false;
    }

    // initialize stuff
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png == nullptr)
    {
        printf("png_create_read_struct failed.\n");
        return false;
    }

    png_infop info = png_create_info_struct(png);
    if (info == nullptr)
    {
        printf("png_create_info_struct failed.\n");
        return false;
    }

    if (setjmp(png_jmpbuf(png)) != 0)
    {
        printf("Error during init_io.\n");
        return false;
    }

    png_init_io(png, (FILE*)file.getHandle());
    png_set_sig_bytes(png, 8);

    png_read_info(png, info);

    unsigned char color_type = png_get_color_type(png, info);
    if (color_type == PNG_COLOR_TYPE_PALETTE)
    {
        png_set_palette_to_rgb(png);
    }

    if (png_get_valid(png, info, PNG_INFO_tRNS))
    {
        png_set_tRNS_to_alpha(png);
    }
    if (png_get_bit_depth(png, info) == 16)
    {
        png_set_strip_16(png);
    }
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
        png_set_gray_to_rgb(png);
    }

    //  int number_of_passes    = png_set_interlace_handling(png);
    png_read_update_info(png, info);

    auto width = png_get_image_width(png, info);
    auto height = png_get_image_height(png, info);
    auto pitch = png_get_rowbytes(png, info);

    // read file
    if (setjmp(png_jmpbuf(png)) != 0)
    {
        printf("Error during read_image.\n");
        return false;
    }

    // create buffer and read data
    png_bytep* row_pointers = new png_bytep[height];
    for (unsigned y = 0; y < height; y++)
    {
        row_pointers[y] = new png_byte[pitch];
    }
    png_read_image(png, row_pointers);

    m_bitmap.width = width;
    m_bitmap.height = height;
    m_bitmap.data.resize(width * height);

    color_type = png_get_color_type(png, info);
    if (color_type == PNG_COLOR_TYPE_RGB)
    {
        auto bitmap = m_bitmap.data.data();
        for (unsigned y = 0; y < height; y++)
        {
            for (unsigned x = 0; x < width; x++)
            {
                const unsigned idx = x * 3;
                *bitmap =
                {
                    *(row_pointers[y] + idx + 0),
                    *(row_pointers[y] + idx + 1),
                    *(row_pointers[y] + idx + 2),
                    255
                };
                bitmap++;
            }

            delete[] row_pointers[y];
        }
    }
    else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
    {
        auto bitmap = m_bitmap.data.data();
        for (unsigned y = 0; y < height; y++)
        {
            for (unsigned x = 0; x < width; x++)
            {
                const unsigned idx = x * 4;
                *bitmap =
                {
                    *(row_pointers[y] + idx + 0),
                    *(row_pointers[y] + idx + 1),
                    *(row_pointers[y] + idx + 2),
                    *(row_pointers[y] + idx + 3)
                };
                bitmap++;
            }

            delete[] row_pointers[y];
        }
    }
    else
    {
        for (unsigned y = 0; y < height; y++)
        {
            delete[] row_pointers[y];
        }
        printf("Should't be happened.\n");
    }

    delete[] row_pointers;

    png_destroy_read_struct(&png, &info, nullptr);

    if (trim != nullptr)
    {
        if (trim->trim(name, m_bitmap))
        {
            m_bitmap = trim->getBitmap();
        }
    }

    return true;
}
