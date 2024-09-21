
#ifndef RAYTRACINGBUTGOOD_IMAGE_TEXTURE_H
#define RAYTRACINGBUTGOOD_IMAGE_TEXTURE_H

#include "interval.h"
#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#include "common.h"
#include <cstdlib>
#include <iostream>
#include <stb_image.h>

class image {
public:
  image() {}

  // tries to load image data. Wants absolute path from executable. Will not do
  // any other folder checking
  image(const char *filename_) {
    auto filename = std::string(filename_);
    if (!load(filename))
      std::cerr << "ERROR! Could not load image file '" << filename_ << "'.\n";
  }

  bool load(const std::string &filename) {
    auto n = bytes_per_pixel;
    fdata = stbi_loadf(filename.c_str(), &image_width, &image_height, &n,
                       bytes_per_pixel);
    if (fdata == nullptr)
      return false;

    bytes_per_scanline = image_width * bytes_per_pixel;
    convert_to_bytes();
    return true;
  }

  ~image() {
    delete[] bdata;
    STBI_FREE(fdata);
  }

  const unsigned char *pixel_data(int x, int y) const {
    static unsigned char magenta[] = {255, 0, 255};
    if (bdata == nullptr)
      return magenta;

    x = clamp(x, 0, image_width);
    y = clamp(y, 0, image_height);

    return bdata + y * bytes_per_scanline + x * bytes_per_pixel;
  }

  int width() const { return (fdata == nullptr) ? 0 : image_width; }
  int height() const { return (fdata == nullptr) ? 0 : image_height; }

private:
  void convert_to_bytes() {
    int total_bytes = image_width * image_height * bytes_per_pixel;
    bdata = new unsigned char[total_bytes];

    auto *bdataptr = bdata;
    auto *fdataptr = fdata;
    for (int i = 0; i < total_bytes; i++, fdataptr++, bdataptr++)
      *bdataptr = float_to_byte(*fdataptr);
  }
  static unsigned char float_to_byte(float val) {
    if (val <= 0)
      return 0;
    if (val >= 1)
      return 255;
    return static_cast<unsigned char>(256.0f * val);
  }

  const int bytes_per_pixel =
      3; // RGB. Most image files do not have the concept of a A channel
  float *fdata = nullptr;
  unsigned char *bdata = nullptr;
  int image_width = 0;
  int image_height = 0;
  int bytes_per_scanline = 0;
};

class image_texture : public Texture {
public:
  image_texture(const char *filename) : m_image(filename) {}

  color value(point2 uv, const point3 &p) const override {
    if (m_image.height() <= 0)
      return color(1, 0, 1);

    uv.x = interval(0, 1)._clamp(uv.x);
    uv.y = 1.0 - interval(0, 1)._clamp(uv.y); // Flipping here

    auto i = int(uv.x * m_image.width());
    auto j = int(uv.y * m_image.height());
    auto pixel = m_image.pixel_data(i, j);

    auto colour_scale = 1.0 / 255.0;
    return color(colour_scale * pixel[0], colour_scale * pixel[1],
                 colour_scale * pixel[2]);
  }

private:
  image m_image;
};

#endif
