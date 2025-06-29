/**
 * @file ImageJPEG.h
 *
 * Declaration of class Image
 */

#ifndef IMAGE_JPEG_H
#define IMAGE_JPEG_H

#include "Image.h"
#include <vector>
#include <mutex>
#include <shared_mutex>>

class ImageJPEG
{
private:
  // protect access to the image pointer and the corresponding jpeg byte vector
  mutable std::shared_mutex image_mutex;

  //HACK: we wrap the image object here
  naoth::Image* image = nullptr;

  // IDEA: would it make sense to make it a parameter?
  // TODO: experiment with quality
  static const int quality = 75;

  mutable std::vector<uint8_t> jpeg;
  mutable size_t jpeg_size = 0;

public:
  // HACK: wrap the image
  // in the future ImageJPEG should have access to the black board
  void set(naoth::Image& image) {
    std::unique_lock lock(image_mutex);
    this->image = &image;
  }

  const naoth::Image& get() const {
    std::shared_lock lock(image_mutex);
    return *image; 
  }

  void compressYUYV() const;
  void decompressYUYV(const std::string& data, unsigned int width, unsigned int height);

  const uint8_t* getJPEG() const { return jpeg.data(); }
  size_t getJPEGSize() const { return jpeg_size; }

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
