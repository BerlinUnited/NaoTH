/* 
 * File:   SimpleBallColorClassifier.h
 * Author: claas
 * Author: Heinrich Mellmann
 *
 * Created on 15. März 2011, 15:56
 */

#ifndef _SimpleBallColorClassifier_H_
#define _SimpleBallColorClassifier_H_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/Image.h" // just for debug
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/SimpleGoalColorPercept.h"
#include "Representations/Perception/SimpleBallColorPercept.h"
#include "Representations/Perception/Histograms.h"

// Tools
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugImageDrawings.h"

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"

#include "Tools/DoubleCamHelpers.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(SimpleBallColorClassifier)
  REQUIRE(FrameInfo)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(ColorChannelHistograms)
  REQUIRE(ColorChannelHistogramsTop)
  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)
  REQUIRE(SimpleGoalColorPercept)
  REQUIRE(SimpleGoalColorPerceptTop)

  PROVIDE(SimpleBallColorPercept)
  PROVIDE(SimpleBallColorPerceptTop)
END_DECLARE_MODULE(SimpleBallColorClassifier)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class SimpleBallColorClassifier : public  SimpleBallColorClassifierBase
{
public:
  double histV[ColorChannelHistograms::VALUE_COUNT];

  SimpleBallColorClassifier();
  virtual ~SimpleBallColorClassifier(){}

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

    Parameters() : ParameterList("SimpleBallColorClassifierParameters")
    {
      PARAMETER_REGISTER(ballColorMax.y) = 200;
      PARAMETER_REGISTER(ballColorMax.u) = 160;
      PARAMETER_REGISTER(dist2yellow.v) = 12;

      syncWithConfig();

      DebugParameterList::getInstance().add(this);
    }

    ~Parameters()
    {
      DebugParameterList::getInstance().remove(this);
    }

    DoublePixel dist2yellow;

    DoublePixel ballColorMax;
    DoublePixel ballColorMin;
  };

  Parameters ballParams;

  // id of the camera the module is curently running on
  CameraInfo::CameraID cameraID;

  DOUBLE_CAM_REQUIRE(SimpleBallColorClassifier, Image);
  DOUBLE_CAM_REQUIRE(SimpleBallColorClassifier, ColorChannelHistograms);
  DOUBLE_CAM_REQUIRE(SimpleBallColorClassifier, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(SimpleBallColorClassifier, SimpleGoalColorPercept);
 
  DOUBLE_CAM_PROVIDE(SimpleBallColorClassifier, SimpleBallColorPercept);
};

#endif  /* _SimpleBallColorClassifier_H_ */