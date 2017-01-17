/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "imagesaver.h"
#include "bitmap.h"
#include "file.h"

#include <png.h>

cImageSaver::cImageSaver(const Bitmap& bitmap)
    : m_bitmap(bitmap)
{
}

cImageSaver::~cImageSaver()
{
}

bool cImageSaver::save(const char* filename) const
{
    cFile file;
    if (file.open(filename, "wb") == false)
    {
        return false;
    }

    // Create and initialize the png_struct with the desired error handler
    // functions.  If you want to use the default stderr and longjump method,
    // you can supply NULL for the last three parameters.  We also check that
    // the library version is compatible with the one used at compile time,
    // in case we are using dynamically linked libraries.  REQUIRED.
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png == nullptr)
    {
        return false;
    }

    // Allocate/initialize the image information data.  REQUIRED
    png_infop info = png_create_info_struct(png);
    if (info == nullptr)
    {
        png_destroy_write_struct(&png, (png_infopp)nullptr);
        return false;
    }

    // Set error handling.  REQUIRED if you aren't supplying your own
    // error hadnling functions in the png_create_write_struct() call.
    if (setjmp(png_jmpbuf(png)))
    {
        png_destroy_write_struct(&png, (png_infopp)nullptr);
        return false;
    }

    auto width = m_bitmap.width;
    auto height = m_bitmap.height;

    // set up the output control if you are using standard C streams
    png_init_io(png, (FILE*)file.getHandle());

    // Set the image information here.  Width and height are up to 2^31,
    // bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
    // the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
    // PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
    // or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
    // PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
    // currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA
                 , PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    //  /* Optionally write comments into the image */
    //  text_ptr[0].key = "Title";
    //  text_ptr[0].text = "Mona Lisa";
    //  text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
    //  text_ptr[1].key = "Author";
    //  text_ptr[1].text = "Leonardo DaVinci";
    //  text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
    //  text_ptr[2].key = "Description";
    //  text_ptr[2].text = "<long text>";
    //  text_ptr[2].compression = PNG_TEXT_COMPRESSION_zTXt;
    //  png_set_text(png, info, text_ptr, 2);

    // Write the file header information.
    png_write_info(png, info);

    // Write image data
    auto src = m_bitmap.data.data();
    for (unsigned y = 0; y < height; y++)
    {
        auto row = reinterpret_cast<const unsigned char*>(&src[y * width]);
        png_write_row(png, row);
    }

    // It is REQUIRED to call this to finish writing the rest of the file
    png_write_end(png, info);

    png_free_data(png, info, PNG_FREE_ALL, -1);

    // clean up after the write, and free any memory allocated
    png_destroy_write_struct(&png, (png_infopp)nullptr);

    return true;
}
