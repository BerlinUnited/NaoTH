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
#include <shared_mutex>
#include <atomic>

class ImageJPEG
{
private:
  // protect access to the image pointer and the corresponding jpeg byte vector
  mutable std::shared_mutex image_mutex;

  mutable std::atomic<bool> compressionValid = false;

  //HACK: we wrap the image object here
  naoth::Image* image = nullptr;

  // IDEA: would it make sense to make it a parameter?
  // TODO: experiment with quality
  static const int quality = 75;

  mutable std::vector<uint8_t> jpeg;
  mutable size_t jpeg_size = 0;

  // Allow access to privat members to the serializer.
  friend class naoth::Serializer<ImageJPEG>;

public:

  // HACK: wrap the image
  // in the future ImageJPEG should have access to the black board
  void set(naoth::Image& image) {
    std::unique_lock lock(image_mutex);
    this->image = &image;
    // Clear the vector without changing the capacity.
    jpeg.clear();
  }

  void invalidateCompressed() {
    compressionValid = false;
  }

  /**
   * Make sure the JPEG image is compressed. This is marked as const, because it semantically does not change the represented image.
   * But it does change the internal representation of the JPEG image.
   * By making this method accessible, you can transfer the **costly** compression to a different thread.
   * It will return early, when the compressed image was still valid.
   */
  void compressYUYV() const;
  void decompressYUYV(const std::string& data, unsigned int width, unsigned int height);


  size_t getJPEGSize() const {
    std::shared_lock lock(image_mutex);
    return jpeg_size;
  }
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
