#include "Framebuffer.h"
#include "lodepng.h"
#include <cstdint>
#include <iostream>


Framebuffer::Framebuffer(const size_t width, const size_t height) : m_height(height), m_width(width), m_bufferData((uint8_t*)malloc(width * height * bytesPerPixel * sizeof(uint8_t*))){}


Framebuffer::~Framebuffer()
{
    free(m_bufferData);
}

void Framebuffer::setPixel ( const size_t x, const size_t y, const uint8_t r, const uint8_t g, const uint8_t b )
{
    const size_t index = (x + y * m_width) * bytesPerPixel;
    m_bufferData[index] = r;
    m_bufferData[index + 1] = g;
    m_bufferData[index + 2] = b;
}

void Framebuffer::getPixel ( const size_t x, const size_t y, uint8_t& r, uint8_t& g, uint8_t& b )
{
    const size_t index = (x + y * m_width) * bytesPerPixel;
    r = m_bufferData[index];
    g = m_bufferData[index + 1];
    b = m_bufferData[index + 2];
}

void Framebuffer::saveBuffer ( const char* fileName )
{
    unsigned error;
    unsigned char* imageBuffer = (unsigned char*)malloc(m_width * m_height * 4 * sizeof(unsigned char*));
    for(size_t y = 0; y < m_height; y++)
    {
        for(size_t x = 0; x < m_width; x++)
        {
            const size_t index = (x + y * m_width) * 4;
            uint8_t r, g, b;
            getPixel(x, y, r, g, b);
            imageBuffer[index + 0] = r;
            imageBuffer[index + 1] = g;
            imageBuffer[index + 2] = b;
            imageBuffer[index + 3] = 255;
        }
    }

    error = lodepng_encode32_file(fileName, imageBuffer, m_width, m_height);

    if(error)
        std::cout << "Error encoding image into PNG: " << error << std::endl;
}




