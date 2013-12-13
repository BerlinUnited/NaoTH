/* 
 * File:   SimpleGoalColorClassifier.h
 * Author: claas
 *
 */

#ifndef _SimpleGoalColorClassifier_H_
#define _SimpleGoalColorClassifier_H_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/Image.h" // just for debug
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Perception/Histograms.h"
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/BaseColorRegionPercept.h"
#include "Representations/Perception/SimpleGoalColorPercept.h"

// Tools
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/DataStructures/Histogram.h"

#include "Tools/DoubleCamHelpers.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(SimpleGoalColorClassifier)
  REQUIRE(FrameInfo)
  REQUIRE(ColorChannelHistograms)
  REQUIRE(ColorChannelHistogramsTop)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)
  REQUIRE(BaseColorRegionPercept)
  REQUIRE(BaseColorRegionPerceptTop)

  PROVIDE(SimpleGoalColorPercept)
  PROVIDE(SimpleGoalColorPerceptTop)
END_DECLARE_MODULE(SimpleGoalColorClassifier)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class SimpleGoalColorClassifier : public  SimpleGoalColorClassifierBase
{
public:
  SimpleGoalColorClassifier();
  virtual ~SimpleGoalColorClassifier(){}

  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);
  }

private:

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("SimpleGoalColorClassifierParameters")
    {
      //PARAMETER_REGISTER(goalColorMin.y) = 80;
      //PARAMETER_REGISTER(goalColorMax.y) = 200;
      //PARAMETER_REGISTER(goalColorWidth.v) = 64;
      //PARAMETER_REGISTER(dist2green.v) = 12;
      //PARAMETER_REGISTER(strength) = 1.2;
      PARAMETER_REGISTER(vu_offset) = 45;

      syncWithConfig();

      DebugParameterList::getInstance().add(this);
    }

    ~Parameters()
    {
      DebugParameterList::getInstance().remove(this);
    }

    //double strength;

    //DoublePixel dist2green;
    //DoublePixel goalColorWidth;
    //DoublePixel goalColorMax;
    //DoublePixel goalColorMin;

    int vu_offset;
  };

  Parameters goalParams;


// experimental (Claas)
private:
   CameraInfo::CameraID cameraID;

  //double histV[COLOR_CHANNEL_VALUE_COUNT];
  //double histU[COLOR_CHANNEL_VALUE_COUNT];

  Statistics::Histogram<ColorChannelHistograms::VALUE_COUNT> histU;
  Statistics::Histogram<ColorChannelHistograms::VALUE_COUNT> histV;
  Statistics::Histogram<ColorChannelHistograms::VALUE_COUNT> histTopU;
  Statistics::Histogram<ColorChannelHistograms::VALUE_COUNT> histTopV;

  void histogramExperiments();

  DOUBLE_CAM_REQUIRE(SimpleGoalColorClassifier, Image);
  DOUBLE_CAM_REQUIRE(SimpleGoalColorClassifier, ColorChannelHistograms);
  DOUBLE_CAM_REQUIRE(SimpleGoalColorClassifier, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(SimpleGoalColorClassifier, BaseColorRegionPercept);
 
  DOUBLE_CAM_PROVIDE(SimpleGoalColorClassifier, SimpleGoalColorPercept);
};

#endif  /* _SimpleGoalColorClassifier_H_ */