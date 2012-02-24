/* 
 * File:   FieldColorClassifier.h
 * Author: claas
 *
 * Created on 15. März 2011, 15:56
 */

#ifndef FIELDCOLORCLASSIFIER_H
#define  FIELDCOLORCLASSIFIER_H

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/ColorTable64.h"
#include "Representations/Perception/BaseColorRegionPercept.h"
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Infrastructure/FrameInfo.h"

// Tools
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"

//Perception
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/Histogram.h"
#include "Tools/ImageProcessing/FieldColorParameters.h"
//#include "Tools/ImageProcessing/CameraParamCorrection.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugModify.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(FieldColorClassifier)
  REQUIRE(ColoredGrid)
  REQUIRE(Histogram)
  REQUIRE(Image)
  REQUIRE(FrameInfo)
  REQUIRE(BaseColorRegionPercept)

  PROVIDE(FieldColorPercept)
END_DECLARE_MODULE(FieldColorClassifier)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class FieldColorClassifier : public  FieldColorClassifierBase
{
public:
  FieldColorClassifier();
  virtual ~FieldColorClassifier(){}

  /** executes the module */
  void execute();

private:
  double adaptationRate;

  FieldColorParameters fieldParams;

  double weightedSmoothedHistY[COLOR_CHANNEL_VALUE_COUNT];
  double weightedSmoothedHistCb[COLOR_CHANNEL_VALUE_COUNT];
  double weightedHistY[COLOR_CHANNEL_VALUE_COUNT];
  double weightedHistCb[COLOR_CHANNEL_VALUE_COUNT];
  double weightedHistCr[COLOR_CHANNEL_VALUE_COUNT];

  double smoothRungeKutta4(const unsigned int& idx, double* valueArray);

  void runDebugRequests(int weigthedMeanY, int meanY);

};

#endif  /* FIELDCOLORCLASSIFIER_H */

