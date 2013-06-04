/**
 * @file Image.h
 *
 * Declaration of class Image
 */

#ifndef _Image_h_
#define _Image_h_

#include <cstring>
#include <iostream>
#include <string> // contains memcpy

#include "Tools/Math/Common.h"
#include "Tools/Math/Vector3.h"
#include "Tools/ImageProcessing/ImageDrawings.h"

#include "Tools/DataStructures/Serializer.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/ImageProcessing/ImagePrimitives.h"

#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Infrastructure/ShadingCorrection.h"

#define SIZE_OF_YUV422_PIXEL 2

namespace naoth
{
  /**
   * Platform independend definition of an image class
   */
  class Image: public DrawingCanvas, public Printable
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
    void wrapImageDataYUV422(unsigned char* data, const unsigned int& size);

    /** Copy a raw image. */
    void copyImageDataYUV422(unsigned char* data, const unsigned int& size);

    /** raw data coded in YUV422*/
    unsigned char* yuv422;
    
    CameraInfo cameraInfo;
    ShadingCorrection shadingCorrection;

    /** The time relative to the start of the programm when the image was recorded in ms */
    unsigned int timestamp;
    unsigned int currentBuffer;
    unsigned int bufferCount;
    unsigned int bufferFailedCount;
    unsigned int wrongBufferSizeCount;

    double meanBrightness;

    bool possibleImageStuck;
                             
    virtual void print(std::ostream& stream) const;
    virtual void toDataStream(std::ostream& stream) const;
    virtual void fromDataStream(std::istream& stream);
    
    virtual void drawPoint
    (
      const unsigned int& x,
      const unsigned int& y,
      const unsigned char& a,
      const unsigned char& b,
      const unsigned char& c
    );

    /**
     * Get the brightness of a pixel. This is faster than getting all color
     * channels.
     */
    inline unsigned char getCorecctedY(const unsigned int x, const unsigned int y) const
    {
      assert(isInside(x,y));
      return (unsigned char) Math::clamp<unsigned short>((yuv422[2 * (y * cameraInfo.resolutionWidth + x)] * shadingCorrection.get(0, x, y)) >> 10, 0, 255);
    }

    /**
     * Get the brightness of a pixel without any correction. This is faster than getting all color
     * channels.
     */
    inline unsigned char getY(const unsigned int x, const unsigned int y) const
    {
      assert(isInside(x,y));
      return yuv422[2 * (y * cameraInfo.resolutionWidth + x)];
    }

    /**
     * Get a pixel (its color). This does a mapping to the YUV422 array
     * so please make sure not to call it more often than you need it.
     * E.g. cache the pixel and dont call get(x,y).y, get(x,y).u, ...
     * seperatly.
     */
    // TODO: remove unsigned to prevent strange implicite casts
    inline Pixel get(const unsigned int x, const unsigned int y) const
    {
      assert(isInside(x,y));
      register unsigned int yOffset = 2 * (y * cameraInfo.resolutionWidth + x);

      Pixel p;
      p.y = yuv422[yOffset];
      
      // ((x & 1)<<1) = 2 if x is odd and 0 if it's even
      p.u = yuv422[yOffset+1-((x & 1)<<1)];
      p.v = yuv422[yOffset+3-((x & 1)<<1)];

      return p;
    }//end get

   /**
     * Get a pixel (its color). This does a mapping to the YUV422 array
     * so please make sure not to call it more often than you need it.
     * E.g. cache the pixel and dont call get(x,y).y, get(x,y).u, ...
     * seperatly.
     */
    inline void get(const unsigned int x, const unsigned int y, Pixel& p) const
    {
      assert(isInside(x,y));
      register unsigned int yOffset = 2 * (y * cameraInfo.resolutionWidth + x);

      p.y = yuv422[yOffset];      
      // ((x & 1)<<1) = 2 if x is odd and 0 if it's even
      p.u = yuv422[yOffset+1-((x & 1)<<1)];
      p.v = yuv422[yOffset+3-((x & 1)<<1)];
      
      // TODO: remove
      /* 
      if((x & 1) == 0) // == (x % 2 == 0)
      {
        p.u = yuv422[yOffset + 1];
        p.v = yuv422[yOffset + 3];
      }
      else
      {
        p.u = yuv422[yOffset - 1];
        p.v = yuv422[yOffset + 1];
      }*/
   }//end get

    /**
     * Get a pixel (its color). This does a mapping to the YUV422 array
     * so please make sure not to call it more often than you need it.
     * E.g. cache the pixel and dont call get(x,y).y, get(x,y).u, ...
     * seperatly.
     */
    inline Pixel getCorrected(const unsigned int x, const unsigned int y) const
    {
      assert(isInside(x,y));
      register unsigned int yOffset = 2 * (y * cameraInfo.resolutionWidth + x);

      Pixel p;
      p.y = (unsigned char) Math::clamp<unsigned short>((yuv422[yOffset] * shadingCorrection.get(0, x, y)) >> 10, 0, 255);
      
      // ((x & 1)<<1) = 2 if x is odd and 0 if it's even
      p.u = yuv422[yOffset+1-((x & 1)<<1)];
      p.v = yuv422[yOffset+3-((x & 1)<<1)];
      //p.u = (unsigned char) Math::clamp<unsigned short>((yuv422[yOffset+1-((x & 1)<<1)] * shadingCorrection.get(1, x, y)) >> 10, 0, 255);
      //p.v = (unsigned char) Math::clamp<unsigned short>((yuv422[yOffset+3-((x & 1)<<1)] * shadingCorrection.get(2, x, y)) >> 10, 0, 255);
      return p;
    }//end get()

   /**
     * Get a pixel (its color). This does a mapping to the YUV422 array
     * so please make sure not to call it more often than you need it.
     * E.g. cache the pixel and dont call get(x,y).y, get(x,y).u, ...
     * seperatly.
     */
    inline void getCorrected(const unsigned int x, const unsigned int y, Pixel& p) const
    {
      assert(isInside(x,y));
      register unsigned int yOffset = 2 * (y * cameraInfo.resolutionWidth + x);

      p.y = (unsigned char) Math::clamp<unsigned short>((yuv422[yOffset] * shadingCorrection.get(0, x, y)) >> 10, 0, 255);
      
      // ((x & 1)<<1) = 2 if x is odd and 0 if it's even
      p.u = yuv422[yOffset+1-((x & 1)<<1)];
      p.v = yuv422[yOffset+3-((x & 1)<<1)];
      //p.u = (unsigned char) Math::clamp<unsigned short>((yuv422[yOffset+1-((x & 1)<<1)] * shadingCorrection.get(1, x, y)) >> 10, 0, 255);
      //p.v = (unsigned char) Math::clamp<unsigned short>((yuv422[yOffset+3-((x & 1)<<1)] * shadingCorrection.get(2, x, y)) >> 10, 0, 255);
    }//end get

    inline void set(const unsigned int x, const unsigned int y, const Pixel& p)
    {
      assert(isInside(x,y));
      register unsigned int yOffset = 2 * (y * cameraInfo.resolutionWidth + x);
      yuv422[yOffset] = p.y;

      // ((x & 1)<<1) = 2 if x is odd and 0 if it's even
      yuv422[yOffset+1-((x & 1)<<1)] = p.u;
      yuv422[yOffset+3-((x & 1)<<1)] = p.v;

      // TODO: remove
      /*
      if((x & 1) == 0) // == (x % 2 == 0)
      {
        yuv422[yOffset + 1] = p.u;
        yuv422[yOffset + 3] = p.v;
      }
      else
      {
        yuv422[yOffset - 1] = p.u;
        yuv422[yOffset + 1] = p.v;
      }
      */
    }//end set()

    inline void set
    (
      const unsigned int& x,
      const unsigned int& y,
      const unsigned char& yy,
      const unsigned char& cb, // u
      const unsigned char& cr // v
    )
    {
      assert(isInside(x,y));
      register unsigned int yOffset = 2 * (y * cameraInfo.resolutionWidth + x);
      yuv422[yOffset] = yy;

      // ((x & 1)<<1) = 2 if x is odd and 0 if it's even
      yuv422[yOffset+1-((x & 1)<<1)] = cb;
      yuv422[yOffset+3-((x & 1)<<1)] = cr;

      // TODO: remove
      /*
      if((x & 1) == 0) // == (x % 2 == 0)
      {
        yuv422[yOffset + 1] = cb;
        yuv422[yOffset + 3] = cr;
      }
      else
      {
        yuv422[yOffset - 1] = cb;
        yuv422[yOffset + 1] = cr;
      }
      */
    }//end set()
    
    /**
     * test whether a pixel is inside the image
     */
    inline bool isInside(const int x, const int y) const
    { 
      return x >= 0 && x < (int)cameraInfo.resolutionWidth && 
             y >= 0 && y < (int)cameraInfo.resolutionHeight;
    }

    inline unsigned int width() const { return cameraInfo.resolutionWidth; }
    inline unsigned int height() const { return cameraInfo.resolutionHeight; }

    inline unsigned int getIndexSize() const
    {
      return cameraInfo.getSize();
    }

    inline unsigned int getXOffsetFromIndex(const unsigned int i) const
    {
      return i % cameraInfo.resolutionWidth;
    }

    inline unsigned int getYOffsetFromIndex(const unsigned int i) const
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

#endif //_Image_h_
