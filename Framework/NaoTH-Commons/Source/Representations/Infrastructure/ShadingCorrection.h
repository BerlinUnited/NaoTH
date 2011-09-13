/*
 * File:   ShadingCorrection.h
 * Author: CNR
 *
 * Created on 1. Februar 2010
 */

#ifndef _ShadingCorrection_H
#define  _ShadingCorrection_H

#include <string>
#include "Representations/Infrastructure/CameraInfo.h"

using namespace std;

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

    unsigned int* yC;
    CameraInfo::CameraID camID;
    unsigned int width;
    unsigned int height;
    unsigned long size;

    unsigned int getY(unsigned int x, unsigned int y) const;

  };
}

#endif  /* _ShadingCorrection_H */
