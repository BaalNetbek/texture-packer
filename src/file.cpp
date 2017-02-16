/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "file.h"

#include <cstdio>

cFile::cFile()
    : m_file(nullptr)
    , m_size(0)
{
}

cFile::~cFile()
{
    close();
}

bool cFile::open(const char* path, const char* mode)
{
    FILE* file = fopen(path, mode);
    if (file != nullptr)
    {
        m_file = file;

        (void)fseek(file, 0, SEEK_END);
        m_size = ftell(file);
        (void)fseek(file, 0, SEEK_SET);

        return true;
    }

    printf("Can't open '%s'.\n", path);
    return false;
}

void cFile::close()
{
    if (m_file != nullptr)
    {
        fclose((FILE*)m_file);
        m_file = nullptr;
    }
}

int cFile::seek(long offset, int whence) const
{
    return fseek((FILE*)m_file, offset, whence);
}

uint32_t cFile::read(void* ptr, uint32_t size) const
{
    if (m_file != nullptr)
    {
        return fread(ptr, 1, size, (FILE*)m_file);
    }

    return 0;
}

uint32_t cFile::write(void* ptr, uint32_t size) const
{
    if (m_file != nullptr)
    {
        return fwrite(ptr, 1, size, (FILE*)m_file);
    }

    return 0;
}

long cFile::getOffset() const
{
    if (m_file != nullptr)
    {
        return ftell((FILE*)m_file);
    }

    return 0;
}
