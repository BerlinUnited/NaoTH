/**
 * @file Image.h
 * 
 * Declaration of class Image
 */ 

#ifndef __Image_h_
#define __Image_h_

#include <cstring>
#include <iostream>
#include <string> // contains memcpy

#include "naorunner/Tools/Math/Common.h"
#include "naorunner/Tools/Math/Vector3.h"

#include "naorunner/Tools/DataStructures/Printable.h"
#include "naorunner/PlatformInterface/PlatformInterchangeable.h"
#include "naorunner/Tools/ImageProcessing/ImagePrimitives.h"

#include "naorunner/Representations/Infrastructure/CameraInfo.h"

#define SIZE_OF_YUV422_PIXEL 2

/**
 * Platform independend definition of an image class
 */
class Image: public Printable, public PlatformInterchangeable
{

public:


  /**
  * Default constructor.
  */
  Image();

  /**
   * Copy constructor
   */
  Image(const Image& orig);

  Image& operator=(const Image& orig);

  /** destructs an image */
  virtual ~Image();

  /** copy the camera information, and recreate the image data if necessary */
  void setCameraInfo(const CameraInfo& ci);

  /** Wrap a raw image pointer without copying it */
  void wrapImageDataYUV422(unsigned char* data, unsigned int size);

  /** Copy a raw image. */
  void copyImageDataYUV422(unsigned char* data, unsigned int size);

  /** raw data coded in YUV422*/
  unsigned char* yuv422;
  
  CameraInfo cameraInfo;

  virtual void print(ostream& stream) const;
  

  virtual void drawPoint(
    unsigned int x, 
    unsigned int y,
    unsigned char a,
    unsigned char b,
    unsigned char c);

  /**
   * Get the brightness of a pixel. This is faster than getting all color
   * channels.
   */
  inline unsigned char getY(unsigned int x, unsigned int y) const
  {
    return yuv422[2*(y*cameraInfo.resolutionWidth + x)];
  }

  /**
   * Get a pixel (it's color). This does a mapping to the YUV422 array
   * so please make sure not to call it more often than you need it.
   * E.g. cache the pixel and dont call get(x,y).y, get(x,y).u, ...
   * seperatly.
   */
  inline Pixel get(unsigned int x, unsigned int y) const
  {
    register unsigned int yOffset = 2*(y*cameraInfo.resolutionWidth + x);

    Pixel p;
    p.y = yuv422[yOffset];
    if((x & 1) == 0) // == (x % 2 == 0)
    {
      p.u = yuv422[yOffset+1];
      p.v = yuv422[yOffset+3];
    }
    else
    {
      p.u = yuv422[yOffset-1];
      p.v = yuv422[yOffset+1];
    }
    return p;
  }//end get()

  inline void set(unsigned int x, unsigned int y, Pixel p)
  {
    register unsigned int yOffset = 2*(y*cameraInfo.resolutionWidth + x);

    yuv422[yOffset] = p.y;

    if(x % 2 == 0)
    {
      yuv422[yOffset+1] = p.u;
      yuv422[yOffset+3] = p.v;
    }
    else
    {
      yuv422[yOffset-1] = p.u;
      yuv422[yOffset+1] = p.v;
    }

  }//end set()
  
  virtual unsigned int width();
  virtual unsigned int height();

  inline unsigned int getIndexSize() const
  {
    return cameraInfo.size;
  }

  inline unsigned int getXOffsetFromIndex(unsigned int i) const
  {
    return i % (cameraInfo.resolutionWidth);
  }

  inline unsigned int getYOffsetFromIndex(unsigned int i) const
  {
    return i / cameraInfo.resolutionWidth;
  }

private:
  bool selfCreatedImage;

};


#endif //__Image_h_
