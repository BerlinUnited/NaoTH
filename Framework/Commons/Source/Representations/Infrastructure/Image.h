/**
 * @file Image.h
 *
 * Declaration of class Image
 */

#ifndef IMAGE_H
#define IMAGE_H

#include <cstring>
#include <iostream>
#include <string> // contains memcpy

#include "Tools/Math/Common.h"
#include "Tools/Math/Vector2.h"

#include "Tools/DataStructures/Serializer.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/ImageProcessing/ImagePrimitives.h"

#include "Representations/Infrastructure/CameraInfo.h"

#include "Tools/Debug/NaoTHAssert.h"


namespace naoth
{
/**
  * Platform independend definition of an image class
  */
class Image: public Printable
{

private:
  Image(const Image& orig);
  Image& operator=(const Image& orig);

  // indicates whether this instance is responsible for the distruction of the data pointer yuv422
  bool selfCreatedImage;

private:
  /** raw image data in YUV422 format */
  unsigned char* yuv422;

public:
  // TODO: remove it
  CameraInfo cameraInfo;

public:
  Image();
  virtual ~Image();

  /** copy the camera information, and recreate the image data if necessary */
  void setCameraInfo(const CameraInfo& ci);

  /** Wrap a raw image pointer without copying it */
  void wrapImageDataYUV422(unsigned char* data, const unsigned int size);

  /** Copy a raw image. */
  void copyImageDataYUV422(const unsigned char* data, const unsigned int size);

  static const unsigned int PIXEL_SIZE_YUV422 = 2;
  static const unsigned int PIXEL_SIZE_YUV444 = 3;

public: // data members

  /** The time relative to the start of the programm when the image was recorded in ms */
  unsigned int timestamp;

  // NOTE: this is used for debugging the V4lCameraHandler
  // TODO: move it to a separate debug representation
  unsigned int currentBuffer;
  unsigned int bufferCount;
  unsigned int wrongBufferSizeCount;

public: // function members
  inline unsigned int width() const { return cameraInfo.resolutionWidth; }
  inline unsigned int height() const { return cameraInfo.resolutionHeight; }
  inline unsigned char* data() const { return yuv422; }
  inline size_t data_size() const { return width()*height()*PIXEL_SIZE_YUV422; }

  // EXPERIMENTAL: return a reference to an aligned yuv422 pixel in the form: |y0|u|y1|v|
  // NOTE: this means we operate with a half of the resolution,
  //       i.e., x = 2*n and x = 2*n+1 will return the same pixel
  inline const Pixel& getAligned(const unsigned int x, const unsigned int y) const {
    return reinterpret_cast<Pixel*>(yuv422)[(y * cameraInfo.resolutionWidth + x)/2];
  }

  inline unsigned char getY(const unsigned int x, const unsigned int y) const {
    ASSERT(isInside(x,y));
    return yuv422[PIXEL_SIZE_YUV422 * (y * cameraInfo.resolutionWidth + x)];
  }

  inline unsigned char getY_direct(const unsigned int x, const unsigned int y) const {
    //ASSERT(isInside(x,y));
    return yuv422[PIXEL_SIZE_YUV422 * (y * cameraInfo.resolutionWidth + x)];
  }

  inline unsigned char getU(const unsigned int x, const unsigned int y) const {
    ASSERT(isInside(x,y));
    return yuv422[PIXEL_SIZE_YUV422 * (y * cameraInfo.resolutionWidth + x) + 1-((x & 1)<<1)];
  }

  inline unsigned char getV(const unsigned int x, const unsigned int y) const {
    ASSERT(isInside(x,y));
    return yuv422[PIXEL_SIZE_YUV422 * (y * cameraInfo.resolutionWidth + x) + 3-((x & 1)<<1)];
  }

  /**
    * Get a pixel (its color). This does a mapping to the YUV422 array
    * so please make sure not to call it more often than you need it.
    * E.g. cache the pixel and dont call get(x,y).y, get(x,y).u, ...
    * seperatly.
    */
  inline Pixel get(const unsigned int x, const unsigned int y) const
  {
    ASSERT(isInside(x,y));
    unsigned int yOffset = PIXEL_SIZE_YUV422 * (y * cameraInfo.resolutionWidth + x);

    Pixel p;
    p.y = yuv422[yOffset];

    // ((x & 1)<<1) = 2 if x is odd and 0 if it's even
    p.u = yuv422[yOffset+1-((x & 1)<<1)];
    p.v = yuv422[yOffset+3-((x & 1)<<1)];

    return p;
  }

  /**
    * Get a pixel (its color). This does a mapping to the YUV422 array
    * so please make sure not to call it more often than you need it.
    * E.g. cache the pixel and dont call get(x,y).y, get(x,y).u, ...
    * seperatly.
    */
  inline void get(const unsigned int x, const unsigned int y, Pixel& p) const
  {
    ASSERT(isInside(x,y));
    unsigned int yOffset = PIXEL_SIZE_YUV422 * (y * cameraInfo.resolutionWidth + x);

    p.y = yuv422[yOffset];
    // ((x & 1)<<1) = 2 if x is odd and 0 if it's even
    p.u = yuv422[yOffset+1-((x & 1)<<1)];
    p.v = yuv422[yOffset+3-((x & 1)<<1)];
  }

  inline void get_direct(const unsigned int x, const unsigned int y, Pixel& p) const
  {
    unsigned int yOffset = PIXEL_SIZE_YUV422 * (y * cameraInfo.resolutionWidth + x);

    p.y = yuv422[yOffset];
    // ((x & 1)<<1) = 2 if x is odd and 0 if it's even
    p.u = yuv422[yOffset+1-((x & 1)<<1)];
    p.v = yuv422[yOffset+3-((x & 1)<<1)];
  }

  inline void set(const unsigned int x, const unsigned int y, const Pixel& p)
  {
    ASSERT(isInside(x,y));
    unsigned int yOffset = PIXEL_SIZE_YUV422 * (y * cameraInfo.resolutionWidth + x);
    yuv422[yOffset] = p.y;

    // ((x & 1)<<1) = 2 if x is odd and 0 if it's even
    yuv422[yOffset+1-((x & 1)<<1)] = p.u;
    yuv422[yOffset+3-((x & 1)<<1)] = p.v;
  }

  inline void set
  (
    const unsigned int x,
    const unsigned int y,
    const unsigned char yy,
    const unsigned char cb, // u
    const unsigned char cr // v
  )
  {
    ASSERT(isInside(x,y));
    unsigned int yOffset = PIXEL_SIZE_YUV422 * (y * cameraInfo.resolutionWidth + x);
    yuv422[yOffset] = yy;

    // ((x & 1)<<1) = 2 if x is odd and 0 if it's even
    yuv422[yOffset+1-((x & 1)<<1)] = cb;
    yuv422[yOffset+3-((x & 1)<<1)] = cr;
  }

  /**
    * test whether a pixel is inside the image
    */
  inline bool isInside(const unsigned int x, const unsigned int y) const
  { 
    return x < cameraInfo.resolutionWidth &&
           y < cameraInfo.resolutionHeight;
  }

  inline bool isInside(const Vector2i& p) const
  {
    return p.x >= 0 && p.x < (int)cameraInfo.resolutionWidth &&
           p.y >= 0 && p.y < (int)cameraInfo.resolutionHeight;
  }

  virtual void print(std::ostream& stream) const;
};


template<>
class Serializer<Image>
{
public:
  static void serialize(const Image& representation, std::ostream& stream);
  static void deserialize(std::istream& stream, Image& representation);
};


/**
  * @brief An image from a secondary camera
  */
class ImageTop : public Image
{
public:
  virtual ~ImageTop() {}
};

template<>
class Serializer<ImageTop> : public Serializer<Image>
{};

} // end namespace naoth

#endif // IMAGE_H
