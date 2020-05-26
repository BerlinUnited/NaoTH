/**
 * @file ImageJPEG.h
 *
 * Declaration of class Image
 */

#ifndef IMAGE_JPEG_H
#define IMAGE_JPEG_H

#include "Image.h"
#include <vector>

class ImageJPEG
{
private:
  //HACK: we wrap the image object here
  naoth::Image* image = nullptr;

public:
  // HACK: wrap the image
  // in the future ImageJPEG should have access to the black board
  void set(naoth::Image& image) {
    this->image = &image;
  }

  const naoth::Image& get() const { return *image; }

  void compressYUYV() const;
  void decompressYUYV(const std::string& data, unsigned int width, unsigned int height);

  const uint8_t* getJPEG() const { return jpeg.data(); }
  size_t getJPEGSize() const { return jpeg_size; }

private:
  // IDEA: would it make sense to make it a parameter?
  // TODO: experiment with quality
  static const int quality = 75;

  mutable std::vector<uint8_t> jpeg;
  mutable size_t jpeg_size = 0;
};

class ImageJPEGTop: public ImageJPEG {};

namespace naoth
{
template<>
class Serializer<ImageJPEG>
{
public:
  static void serialize(const ImageJPEG& representation, std::ostream& stream);
  static void deserialize(std::istream& stream, ImageJPEG& representation);
};

template<> class Serializer<ImageJPEGTop> : public Serializer<ImageJPEG> {};

} // end namespace naoth

#endif //_ImageJPEG_h_
