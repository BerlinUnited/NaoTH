#ifndef BASECOLORREGIONPARAMETERS_H
#define BASECOLORREGIONPARAMETERS_H

#include <Tools/ImageProcessing/ImagePrimitives.h>
#include "Tools/Debug/DebugParameterList.h"

using namespace naoth;

class BaseColorRegionParameters : public ParameterList
{
public:
  int goalVUdistance;
  int goalVUdistanceMin;
  int goalVUdistanceMax;

  DoublePixel fieldIndex;
  DoublePixel fieldDist;

  DoublePixel goalIndex;
  DoublePixel goalDist;

  DoublePixel ballIndex;
  DoublePixel ballDist;

  DoublePixel lineIndex;
  DoublePixel lineDist;

  BaseColorRegionParameters();
  ~BaseColorRegionParameters();
};


#endif // BASECOLORREGIONPARAMETERS_H
