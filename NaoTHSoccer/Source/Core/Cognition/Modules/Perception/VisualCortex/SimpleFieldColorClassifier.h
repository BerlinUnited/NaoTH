/* 
 * File:   SimpleFieldColorClassifier.h
 * Author: claas
 * Author: Heinrich Mellmann
 *
 * Created on 15. März 2011, 15:56
 */

#ifndef _SimpleFieldColorClassifier_H_
#define _SimpleFieldColorClassifier_H_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/Histogram.h"
#include "Tools/ImageProcessing/FieldColorParameters.h"


// Tools
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include <Tools/DataStructures/ParameterList.h>


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(SimpleFieldColorClassifier)
  REQUIRE(ColoredGrid)
  REQUIRE(Histogram)
  REQUIRE(Image)
  REQUIRE(FrameInfo)

  PROVIDE(FieldColorPercept)
END_DECLARE_MODULE(SimpleFieldColorClassifier)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class SimpleFieldColorClassifier : public  SimpleFieldColorClassifierBase
{
public:
  SimpleFieldColorClassifier();
  virtual ~SimpleFieldColorClassifier(){}

  /** executes the module */
  void execute();

private:

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("SimpleFieldColorClassifierParameters")
    {
      PARAMETER_REGISTER(fieldcolorDistMin.y) = 20;
      PARAMETER_REGISTER(fieldcolorDistMin.u) = 5;
      PARAMETER_REGISTER(fieldcolorDistMin.v) = 5;

      PARAMETER_REGISTER(fieldcolorDistMax.y) = 72;
      PARAMETER_REGISTER(fieldcolorDistMax.u) = 32;
      PARAMETER_REGISTER(fieldcolorDistMax.v) = 24;

      syncWithConfig();

      DebugParameterList::getInstance().add(this);
    }

    ~Parameters()
    {
      DebugParameterList::getInstance().remove(this);
    }

    DoublePixel fieldcolorDistMin;
    DoublePixel fieldcolorDistMax;
  };

  FieldColorParameters fieldParams;
/*
  double weightedHistY[COLOR_CHANNEL_VALUE_COUNT];
  double weightedHistCb[COLOR_CHANNEL_VALUE_COUNT];
  double weightedHistCr[COLOR_CHANNEL_VALUE_COUNT];
  */
};

#endif  /* _SimpleFieldColorClassifier_H_ */

