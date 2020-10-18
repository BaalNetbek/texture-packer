/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <cstdint>
#include <string.h>
#include <vector>

class cBitmap
{
public:
    ~cBitmap()
    {
        clear();
    }

    void clear()
    {
        m_width = 0;
        m_height = 0;

        if (m_manageData)
        {
            m_manageData = false;
            delete[] m_data;
        }

        m_data = nullptr;
    }

    void createBitmap(uint32_t width, uint32_t height)
    {
        clear();

        m_width = width;
        m_height = height;

        m_manageData = true;
        m_data = new Pixel[width * height];
    }

    void setBitmap(uint32_t width, uint32_t height, void* data)
    {
        clear();

        m_width = width;
        m_height = height;

        m_data = static_cast<Pixel*>(data);
    }

    cBitmap& operator=(const cBitmap& other)
    {
        if (this != &other)
        {
            createBitmap(other.m_width, other.m_height);
            std::copy(other.getData(), other.getData() + other.m_width * other.m_height, m_data);
        }

        return *this;
    }

    template<typename T>
    void moveAndSet(T& me, T& other, T value)
    {
        me = other;
        other = value;
    }

    cBitmap& operator=(cBitmap&& other) noexcept
    {
        if(this != &other)
        {
            clear();

            moveAndSet(m_width, other.m_width, 0u);
            moveAndSet(m_height, other.m_height, 0u);

            moveAndSet(m_manageData, other.m_manageData, false);
            moveAndSet(m_data, other.m_data, static_cast<Pixel*>(nullptr));
        }

        return *this;
    }

    uint32_t getWidth() const
    {
        return m_width;
    }

    uint32_t getHeight() const
    {
        return m_height;
    }

    struct Pixel
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    Pixel* getData()
    {
        return m_data;
    }

    const Pixel* getData() const
    {
        return m_data;
    }

private:
    uint32_t m_width = 0;
    uint32_t m_height = 0;

    bool m_manageData = false;
    Pixel* m_data = nullptr;
};
