/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "Bitmap.h"

#include <algorithm>

cBitmap::~cBitmap()
{
    clear();
}

void cBitmap::clear()
{
    m_size = { 0u, 0u };

    if (m_manageData)
    {
        m_manageData = false;
        delete[] m_data;
    }

    m_data = nullptr;
}

void cBitmap::createBitmap(const sSize& size)
{
    clear();

    m_size = size;

    m_manageData = true;
    m_data = new Pixel[size.width * size.height];
}

void cBitmap::setBitmap(const sSize& size, void* data)
{
    clear();

    m_size = size;

    m_data = static_cast<Pixel*>(data);
}

cBitmap& cBitmap::operator=(const cBitmap& other)
{
    if (this != &other)
    {
        createBitmap(other.m_size);
        std::copy(other.getData(), other.getData() + other.m_size.width * other.m_size.height, m_data);
    }

    return *this;
}

cBitmap& cBitmap::operator=(cBitmap&& other) noexcept
{
    if (this != &other)
    {
        clear();

        moveAndSet(m_size, other.m_size, {});

        moveAndSet(m_manageData, other.m_manageData, false);
        moveAndSet(m_data, other.m_data, static_cast<Pixel*>(nullptr));
    }

    return *this;
}
