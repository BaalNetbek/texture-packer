/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

class cFile final
{
public:
    cFile();
    ~cFile();

    bool open(const char* path, const char* mode = "rb");
    void close();

    long getOffset() const;
    void* getHandle() const { return m_file; }

    virtual int seek(long offset, int whence) const;
    virtual unsigned read(void* ptr, unsigned size) const;
    virtual unsigned write(void* ptr, unsigned size) const;
    virtual long getSize() const { return m_size; }

protected:
    void* m_file;
    long m_size;
};
