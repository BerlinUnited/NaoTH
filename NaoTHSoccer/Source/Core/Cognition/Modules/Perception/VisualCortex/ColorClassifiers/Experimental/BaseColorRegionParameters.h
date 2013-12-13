#ifndef BASECOLORREGIONPARAMETERS_H
#define BASECOLORREGIONPARAMETERS_H

#include <Tools/ImageProcessing/ImagePrimitives.h>
#include <Tools/ImageProcessing/ColorRegion.h>

using namespace naoth;

class BaseColorRegionParameters
{
public:
  whiteParamColorRegion whiteLineParamsTop;
  whiteParamColorRegion whiteLineParams;
  yellowParamColorRegion yellowGoalParamsTop;
  yellowParamColorRegion yellowGoalParams;
  skyblueParamColorRegion blueGoalParamsTop;
  skyblueParamColorRegion blueGoalParams;
  orangeParamColorRegion orangeBallParamsTop;
  orangeParamColorRegion orangeBallParams;
  pinkParamColorRegion redTeamParamsTop;
  pinkParamColorRegion redTeamParams;
  blueParamColorRegion blueTeamParamsTop;
  blueParamColorRegion blueTeamParams;

  BaseColorRegionParameters();
  ~BaseColorRegionParameters(){};
};


#endif // BASECOLORREGIONPARAMETERS_H
