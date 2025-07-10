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
#include <future>

struct JpegData {
  std::vector<uint8_t> bytes;
  size_t number_of_bytes = 0;
  unsigned int width;
  unsigned int height;
};

class ImageJPEG
{
private:
  // protect access to the image pointer and the corresponding jpeg byte vector
  mutable std::mutex image_mutex;
  mutable std::future<JpegData> futureJpegData;
  mutable JpegData jpegData;

  //HACK: we wrap the image object here
  naoth::Image* image = nullptr;
  
  // IDEA: would it make sense to make it a parameter?
  // TODO: experiment with quality
  static const int quality = 75;

public:

  // HACK: wrap the image
  // in the future ImageJPEG should have access to the black board
  void set(naoth::Image& image) {
    {
      std::unique_lock lock(image_mutex);
      this->image = &image;
    }
    compressImageAsync();
  }

  /**
   * Get the compressed JPEG data.
   * This waits for the background job to finish.
   */
  JpegData& get() const {
    // Wait for the future and set the parent representation when finished
    if(futureJpegData.valid()) {
      futureJpegData.wait();

      std::unique_lock lock(image_mutex);
      if(futureJpegData.valid()) {
        jpegData = futureJpegData.get();
        jpegData.height = image->height();
        jpegData.width = image->width();
      }
    }
    return jpegData;
  }

  void compressImageAsync();

  void decompressYUYV(const std::string& data, unsigned int width, unsigned int height);

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
