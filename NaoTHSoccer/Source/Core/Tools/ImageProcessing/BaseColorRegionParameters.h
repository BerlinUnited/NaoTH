#ifndef BASECOLORREGIONPARAMETERS_H
#define BASECOLORREGIONPARAMETERS_H

#include <Tools/ImageProcessing/ImagePrimitives.h>
#include <Tools/ImageProcessing/ColorRegion.h>

using namespace naoth;

class BaseColorRegionParameters
{
public:
  whiteParamColorRegion whiteLineParams;
  yellowParamColorRegion yellowGoalParams;
  skyblueParamColorRegion blueGoalParams;
  orangeParamColorRegion orangeBallParams;
  pinkParamColorRegion pinkWaistBandParams;
  blueParamColorRegion blueWaistBandParams;

  BaseColorRegionParameters();
  ~BaseColorRegionParameters(){};
};


#endif // BASECOLORREGIONPARAMETERS_H
