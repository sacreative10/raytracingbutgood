#ifndef FRAMEBUFFER_H_INCLUDED
#define FRAMEBUFFER_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

/**
 * @todo write docs
 */
class Framebuffer
{
public:
    static constexpr size_t bytesPerPixel = 3;
    Framebuffer(const size_t width, const size_t height);
    ~Framebuffer();

    void setPixel(const size_t x, const size_t y, const uint8_t r, const uint8_t g, const uint8_t b);
    void getPixel(const size_t x, const size_t y, uint8_t& r, uint8_t& g, uint8_t& b);

    void saveBuffer(const char* fileName);

    inline size_t getHeight(){return m_height;}
    inline size_t getWidth(){return m_width;}
private:
    size_t m_height;
    size_t m_width;
    uint8_t* m_bufferData;
};

#endif // FRAMEBUFFER_H

#endif // FRAMEBUFFER_H_INCLUDED
