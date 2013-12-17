/*
 * File:   ShadingCorrection.h
 * Author: CNR
 *
 * Created on 1. Februar 2010
 */

#ifndef _ShadingCorrection_H
#define _ShadingCorrection_H

#include <string>
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Infrastructure/Image.h"

class ShadingCorrection
{
public:

  ShadingCorrection();
  ~ShadingCorrection();

  bool loadCorrectionFromFiles(std::string camConfigPath, std::string hardwareID );
  bool loadCorrectionFile(std::string filePath, unsigned int idx);
  void saveCorrectionToFiles(std::string camConfigPath, std::string hardwareID );
  void saveCorrectionFile(std::string filePath, unsigned int idx);

  void init(unsigned int w, unsigned int h, naoth::CameraInfo::CameraID cam);
  void reset();
  void reset(unsigned int idx);
  void clear();

  inline unsigned int get(unsigned int idx, unsigned int x, unsigned int y) const
  {
    //if not initialized or invalid pixel, return default value
    if(x > width || y > height || idx > 2 || data[idx] == NULL )
    {
      return 1024;
    }
    return data[idx][y * width + x];
  }//end get

  inline unsigned int get(unsigned int idx, unsigned int i) const
  {
    //if not initialized or invalid pixel, return default value
    if(i >= size || idx > 2 || data[idx] == NULL)
    {
      return 1024;
    }
    return data[idx][i];
  }//end get

  inline void set(unsigned int idx, unsigned int x, unsigned int y, unsigned short value)
  {
    //if not initialized or invalid pixel, return default value
    if(x > width || y > height || idx > 2 || data[idx] == NULL )
    {
      return;
    }
    data[idx][y * width + x] = value;
  }//end get

  inline void set(unsigned int idx, unsigned int i, unsigned short value)
  {
    //if not initialized or invalid pixel, do nothing
    if(i >= size || idx > 2 || data[idx] == NULL )
    {
      return;
    }
    data[idx][i] = value;
  }//end setY

  inline unsigned short* getDataPointer(unsigned int idx) const
  {
    return data[idx];
  }

  inline unsigned long getSize()const
  {
    return size;
  }

  /**
    * Get the brightness of a pixel. This is faster than getting all color
    * channels.
    */
  inline unsigned char getCorecctedY(const naoth::Image& image, const int x, const int y) const
  {
    int v = (image.getY(x,y) * get(0, x, y)) >> 10;
    return (unsigned char) Math::clamp<int>(v, 0, 255);
  }

  /**
    * Get a pixel (its color). This does a mapping to the YUV422 array
    * so please make sure not to call it more often than you need it.
    * E.g. cache the pixel and dont call get(x,y).y, get(x,y).u, ...
    * seperatly.
    */
  inline Pixel getCorrectedPixel(const naoth::Image& image, const int x, const int y) const
  {
    ASSERT(image.isInside(x,y));
    //register unsigned int yOffset = 2 * (y * image.cameraInfo.resolutionWidth + x);

    Pixel p = image.get(x,y);
    p.y = (unsigned char) Math::clamp<int>((p.y * get(0, x, y)) >> 10, 0, 255);
    p.u = (unsigned char) Math::clamp<int>((p.y * get(1, x, y)) >> 10, 0, 255);
    p.v = (unsigned char) Math::clamp<int>((p.y * get(2, x, y)) >> 10, 0, 255);
    return p;
  }

  inline void getCorrectedPixel(const naoth::Image& image, const int x, const int y, Pixel& p) const
  {
    ASSERT(image.isInside(x,y));
    //register unsigned int yOffset = 2 * (y * image.cameraInfo.resolutionWidth + x);

    p = image.get(x,y);
    p.y = (unsigned char) Math::clamp<int>((p.y * get(0, x, y)) >> 10, 0, 255);
    p.u = (unsigned char) Math::clamp<int>((p.y * get(1, x, y)) >> 10, 0, 255);
    p.v = (unsigned char) Math::clamp<int>((p.y * get(2, x, y)) >> 10, 0, 255);
  }

private:
  unsigned short* data[3];
  naoth::CameraInfo::CameraID camID;
  unsigned int width;
  unsigned int height;
  unsigned long size;
};


#endif  /* _ShadingCorrection_H */
