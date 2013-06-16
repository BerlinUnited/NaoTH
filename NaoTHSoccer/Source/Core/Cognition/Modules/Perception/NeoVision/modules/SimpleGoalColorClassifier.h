/* 
 * File:   SimpleGoalColorClassifier.h
 * Author: claas
 *
 */

#ifndef _SimpleGoalColorClassifier_H_
#define _SimpleGoalColorClassifier_H_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/BaseColorRegionPercept.h"
#include "Representations/Perception/SimpleGoalColorPercept.h"
#include "Representations/Infrastructure/FrameInfo.h"

#include "Representations/Infrastructure/Image.h" // just for debug


// Tools
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/ImageProcessing/Histograms.h"
#include "Tools/DataStructures/Histogram.h"


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(SimpleGoalColorClassifier)
  REQUIRE(FrameInfo)
  REQUIRE(Histograms)
  REQUIRE(HistogramsTop)
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
  //double histV[COLOR_CHANNEL_VALUE_COUNT];
  //double histU[COLOR_CHANNEL_VALUE_COUNT];
  
  SimpleGoalColorClassifier();
  virtual ~SimpleGoalColorClassifier(){}

  /** executes the module */
  void execute();

private:
  Statistics::Histogram<COLOR_CHANNEL_VALUE_COUNT> histU;
  Statistics::Histogram<COLOR_CHANNEL_VALUE_COUNT> histV;
  Statistics::Histogram<COLOR_CHANNEL_VALUE_COUNT> histTopU;
  Statistics::Histogram<COLOR_CHANNEL_VALUE_COUNT> histTopV;

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("SimpleGoalColorClassifierParameters")
    {
      PARAMETER_REGISTER(goalColorMin.y) = 80;
      PARAMETER_REGISTER(goalColorMax.y) = 200;
      PARAMETER_REGISTER(goalColorWidth.v) = 64;
      PARAMETER_REGISTER(dist2green.v) = 12;
      PARAMETER_REGISTER(strength) = 1.2;

      syncWithConfig();

      DebugParameterList::getInstance().add(this);
    }

    ~Parameters()
    {
      DebugParameterList::getInstance().remove(this);
    }

    double strength;

    DoublePixel dist2green;

    DoublePixel goalColorWidth;

    DoublePixel goalColorMax;
    DoublePixel goalColorMin;
  };

  Parameters goalParams;

};

#endif  /* _SimpleGoalColorClassifier_H_ */