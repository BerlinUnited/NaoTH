/**
 * @file ImageJPEG.h
 *
 * Declaration of class Image
 */

#ifndef _ImageJPEG_h_
#define _ImageJPEG_h_

#include "Image.h"
#include <vector>

class ImageJPEG {
private:
  const naoth::Image* image = NULL;

public:
  // wrap the image
  void set(const naoth::Image& image) { this->image = &image; }
  const naoth::Image& get() const { return *image; }

  inline void compress() const;

  unsigned char* getJPEG() const { return jpeg; }
  unsigned long getJPEGSize() const { return jpeg_size; }

private:
  int quality = 50;

  mutable std::vector<uint8_t> linebuf; // needed by compress
  mutable std::vector<uint8_t> planar; // planar image for compressTurbo

  // output of compress and compressTurbo
  mutable unsigned char* jpeg = NULL;
  mutable unsigned long jpeg_size = 0;
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

template<>
class Serializer<ImageJPEGTop> : public Serializer<ImageJPEG>
{};

} // end namespace naoth

#endif //_ImageJPEG_h_
