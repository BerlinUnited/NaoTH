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
#include "PlatformInterface/Platform.h"


namespace naoth
{
  class ShadingCorrection
  {
  public:

    ShadingCorrection();
    ~ShadingCorrection();

    bool loadCorrectionFromFile(string camConfigPath, string hardwareID );
    void saveCorrectionToFile(string camConfigPath, string hardwareID );
    void init(unsigned int w, unsigned int h, CameraInfo::CameraID cam);
    void reset();
    void clear();

    inline unsigned int getY(unsigned int x, unsigned int y) const
    {
      //if not initialized or invalid pixel, return default value
      if(x > width || y > height || yC == NULL )
      {
        return 1024;
      }
      return yC[y * width + x];
    }//end getY

    inline unsigned int getY(unsigned int i) const
    {
      //if not initialized or invalid pixel, return default value
      if(i >= size || yC == NULL )
      {
        return 1024;
      }
      return yC[i];
    }//end getY

    inline void setY(unsigned int i, unsigned int value)
    {
      //if not initialized or invalid pixel, do nothing
      if(i >= size || yC == NULL )
      {
        return;
      }
      yC[i] = value;
    }//end setY

    inline void setY(unsigned int x, unsigned int y, unsigned int value)
    {
      //if not initialized or invalid pixel, return default value
      if(x > width || y > height || yC == NULL )
      {
        return;
      }
      yC[y * width + x] = value;
    }//end getY

  private:
    unsigned int* yC;
    CameraInfo::CameraID camID;
    unsigned int width;
    unsigned int height;
    unsigned long size;
  };
}

#endif  /* _ShadingCorrection_H */
