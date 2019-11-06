/**
 * @file ImageJPEG.h
 *
 * Declaration of class Image
 */

#ifndef _ImageJPEG_h_
#define _ImageJPEG_h_

#include "Image.h"
#include <vector>
#include <turbojpeg/jpeglib.h>

#include <Representations/Debug/Stopwatch.h>

class ImageJPEG 
{
private:
  const naoth::Image* image = NULL;

public:
  // HACK: wrap the image
  // in the future ImageJPEG should have access to the black board
  void set(const naoth::Image& image) { 
    this->image = &image; 
  }

  const naoth::Image& get() const { return *image; }

  void compress() const;
  void compressYUYV() const;

  //std::string& getJPEGString() const { return jpeg_str; }
  unsigned char* getJPEG() const { return jpeg_data.data(); /*jpeg;*/ }
  unsigned long getJPEGSize() const { return jpeg_size; }


private:
  // IDEA: would it make sense to make it a parameter?
  // TODO: experiment with quality
  static const int quality = 75;

  mutable std::vector<uint8_t> linebuf; // needed by compress
  mutable std::vector<uint8_t> planar; // planar image for compressTurbo

  // output of compress and compressTurbo
  mutable unsigned char* jpeg = NULL;
  mutable unsigned long jpeg_size = 0;

  mutable std::vector<uint8_t> jpeg_data;
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
