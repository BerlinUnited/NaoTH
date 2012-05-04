#ifndef BASECOLORREGIONPARAMETERS_H
#define BASECOLORREGIONPARAMETERS_H

#include <Tools/ImageProcessing/ImagePrimitives.h>
#include <Tools/ImageProcessing/ColorRegion.h>
#include "Tools/Debug/DebugParameterList.h"

using namespace naoth;

class BaseColorRegionParameters : public ParameterList
{
public:
  ColorRegionParameter fieldParams;
  ColorRegionParameter linesParams;
  yellowParamColorRegion yellowGoalParams;
  skyblueParamColorRegion blueGoalParams;
  orangeParamColorRegion orangeBallParams;
  pinkParamColorRegion pinkWaistBandParams;
  blueParamColorRegion blueWaistBandParams;

  PixelT<double> distGray;
  PixelT<double> grayLevel;

  int goalVUdistance;
  int goalVUdistanceMin;
  int goalVUdistanceMax;

  PixelT<double> fieldIndex;
  PixelT<double> fieldDist;

  PixelT<double> goalIndex;
  PixelT<double> goalDist;

  PixelT<double> ballIndex;
  PixelT<double> ballDist;

  PixelT<double> lineIndex;
  PixelT<double> lineDist;

  BaseColorRegionParameters();
  ~BaseColorRegionParameters();
};


#endif // BASECOLORREGIONPARAMETERS_H
