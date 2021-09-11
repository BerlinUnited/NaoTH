#ifndef MultiChannelIntegralImage_H
#define MultiChannelIntegralImage_H

#include <stdint.h>
#include <sys/types.h>
#include <string.h>

#include <Tools/Math/Vector2.h>
#include <Tools/Debug/NaoTHAssert.h>


class MultiChannelIntegralImage 
{
public:
  
  MultiChannelIntegralImage() : 
    integralImage(nullptr),
    width(0),
    height(0) 
  {}
  
  ~MultiChannelIntegralImage() {
    delete[] integralImage;
  }

  void setDimension(uint32_t _width, uint32_t _height) {
    if (height != _height || width != _width) {
      delete[] integralImage;
      width = _width;
      height = _height;

      // this should initialize the array to 0
      integralImage = new uint32_t[height*width*MAX_COLOR];
      memset(integralImage, 0, height*width*MAX_COLOR*sizeof(uint32_t));
    }
  }

  // TODO: this seems obsolete
  bool isValid() const {
    return integralImage != nullptr;
  }

  uint32_t* getDataPointer() {
    return integralImage;
  }

  uint32_t getSumForRect(uint32_t minX, uint32_t minY, uint32_t maxX, uint32_t maxY, uint32_t c) const {

    ASSERT(maxX < width);
    ASSERT(maxY < height);

    const uint32_t idx1 = (minY * width + minX)*MAX_COLOR;
    const uint32_t idx2 = (minY * width + maxX)*MAX_COLOR;
    const uint32_t idx3 = (maxY * width + minX)*MAX_COLOR;
    const uint32_t idx4 = (maxY * width + maxX)*MAX_COLOR;

    ASSERT(nullptr != integralImage);
    const uint32_t *buffer1 = integralImage + idx1;
    const uint32_t *buffer2 = integralImage + idx2;
    const uint32_t *buffer3 = integralImage + idx3;
    const uint32_t *buffer4 = integralImage + idx4;

    const uint32_t sum = buffer1[c] + buffer4[c] - buffer2[c] - buffer3[c];
    return sum;
  }

  double getDensityForRect(uint32_t minX, uint32_t minY, uint32_t maxX, uint32_t maxY, uint32_t c) const {
    uint32_t count = getSumForRect(minX, minY, maxX, maxY, c);
    return double(count) / double((maxX-minX)*(maxY-minY));
  }

  uint32_t getWidth() const {
    return width;
  }

  uint32_t getHeight() const {
    return height;
  }

public:
  static const uint32_t MAX_COLOR = 3;
  static const int32_t FACTOR = 4;

private:
  uint32_t* integralImage;
  uint32_t width, height;
};


class GameColorIntegralImage : public MultiChannelIntegralImage { };
class GameColorIntegralImageTop : public GameColorIntegralImage { };

class BallDetectorIntegralImage : public MultiChannelIntegralImage { };
class BallDetectorIntegralImageTop : public BallDetectorIntegralImage { };

#endif
