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

    bool loadCorrectionFromFiles(string camConfigPath, string hardwareID );
    bool loadCorrectionFile(string filePath, unsigned int idx);
    void saveCorrectionToFiles(string camConfigPath, string hardwareID );
    void saveCorrectionFile(string filePath, unsigned int idx);

    void init(unsigned int w, unsigned int h, CameraInfo::CameraID cam);
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

    inline void set(unsigned int idx, unsigned int x, unsigned int y, unsigned int value)
    {
      //if not initialized or invalid pixel, return default value
      if(x > width || y > height || idx > 2 || data[idx] == NULL )
      {
        return;
      }
      data[idx][y * width + x] = value;
    }//end get

    inline void set(unsigned int idx, unsigned int i, unsigned int value)
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


  private:
    unsigned short* data[3];
    CameraInfo::CameraID camID;
    unsigned int width;
    unsigned int height;
    unsigned long size;
  };
}

#endif  /* _ShadingCorrection_H */
