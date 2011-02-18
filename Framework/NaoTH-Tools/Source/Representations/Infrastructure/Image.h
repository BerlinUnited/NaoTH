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

#include "Tools/Math/Common.h"
#include "Tools/Math/Vector3.h"
#include "Tools/ImageProcessing/ImageDrawings.h"

#include "Tools/DataStructures/Streamable.h"
#include "Tools/DataStructures/Serializer.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/ImageProcessing/ImagePrimitives.h"
#include "PlatformInterface/PlatformInterchangeable.h"

#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Infrastructure/ShadingCorrection.h"

#define SIZE_OF_YUV422_PIXEL 2

namespace naoth
{
  /**
   * Platform independend definition of an image class
   */
  class Image: public Printable, public PlatformInterchangeable, public Streamable, public DrawingCanvas
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
    ShadingCorrection shadingCorrection;

    /** The time relative to the start of the programm when the image was recorded in ms */
    unsigned int timestamp;
    unsigned int currentBuffer;
    unsigned int bufferCount;
    unsigned int bufferFailedCount;


    virtual void print(ostream& stream) const;
    virtual void toDataStream(ostream& stream) const;
    virtual void fromDataStream(istream& stream);


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
      return (yuv422[2 * (y * cameraInfo.resolutionWidth + x)] * shadingCorrection.getY(x,y)) >> 10;// * yC[y * cameraInfo.resolutionWidth + x]) >> 10;
    }

    /**
     * Get a pixel (its color). This does a mapping to the YUV422 array
     * so please make sure not to call it more often than you need it.
     * E.g. cache the pixel and dont call get(x,y).y, get(x,y).u, ...
     * seperatly.
     */
    inline Pixel get(unsigned int x, unsigned int y) const
    {
      register unsigned int yOffset = 2 * (y * cameraInfo.resolutionWidth + x);

      Pixel p;
      p.y = (yuv422[yOffset] * shadingCorrection.getY(x,y)) >> 10;// * yC[y * cameraInfo.resolutionWidth + x]) >> 10;
      /* TODO: unify it to: ((x & 1)<<1) = 2 if x is odd and 0 if it's even
       p.u = yuv422[yOffset+1-((x & 1)<<1)];
       p.v = yuv422[yOffset+3-((x & 1)<<1)];
       */
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
      register unsigned int yOffset = 2 * (y * cameraInfo.resolutionWidth + x);

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

  template<>
  class Serializer<Image>
  {
  public:
    static void serialize(const Image& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, Image& representation);
  };

}

#endif //__Image_h_
