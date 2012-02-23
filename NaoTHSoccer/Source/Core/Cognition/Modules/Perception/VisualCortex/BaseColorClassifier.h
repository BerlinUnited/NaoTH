#ifndef BASECOLORCLASSIFIER_H
#define BASECOLORCLASSIFIER_H

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/ColorTable64.h"
#include "Representations/Perception/BaseColorRegionPercept.h"
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Infrastructure/FrameInfo.h"

// Tools
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include "Tools/DataStructures/RingBufferWithSum.h"

//Perception
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/Histogram.h"
#include "Tools/ImageProcessing/BaseColorRegionParameters.h"
//#include "Tools/ImageProcessing/CameraParamCorrection.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugModify.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(BaseColorClassifier)
  REQUIRE(ColoredGrid)
  REQUIRE(Histogram)
  REQUIRE(Image)
  REQUIRE(FrameInfo)
  REQUIRE(FieldColorPercept)
  REQUIRE(GoalPercept)

  PROVIDE(BaseColorRegionPercept)
  PROVIDE(ColorTable64)
END_DECLARE_MODULE(BaseColorClassifier)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class BaseColorClassifier : public  BaseColorClassifierBase
{
public:
  BaseColorClassifier();
  virtual ~BaseColorClassifier(){}

  /** executes the module */
  void execute();

  void initPercept();
  void setPercept();

private:
  double adaptationRate;

  RingBufferWithSum<double, 100> meanY;
  RingBufferWithSum<double, 100> meanU;
  RingBufferWithSum<double, 100> meanV;

  RingBufferWithSum<double, 100> goalMeanY;
  RingBufferWithSum<double, 100> goalMeanU;
  RingBufferWithSum<double, 100> goalMeanV;
  bool goalIsCalibrating;

  double lastMeanY;
  double lastMeanU;
  double lastMeanV;

  void runDebugRequests();

  BaseColorRegionParameters regionParams;
  BaseColorRegionPercept& bPercept;
  ColorTable64& cTable;
  const FieldColorPercept& fPercept;
  const Histogram& histogram;
  const ColoredGrid& coloredGrid;


};

#endif // BASECOLORCLASSIFIER_H
