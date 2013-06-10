#ifndef FIELDCOLORCLASSIFIERFULL_H
#define FIELDCOLORCLASSIFIERFULL_H

/*
 * File:   FieldColorClassifierFull.h
 * Author: claas
 *
 * Created on 15. März 2011, 15:56
 */

#include <ios>

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
#include "Tools/ImageProcessing/Histograms.h"
//#include "Tools/ImageProcessing/FieldColorParametersFull.h"
//#include "Tools/ImageProcessing/CameraParamCorrection.h"
#include "Tools/DataStructures/RingBufferWithSum.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(FieldColorClassifierFull)
  REQUIRE(ColoredGrid)
  REQUIRE(Histograms)
  REQUIRE(Image)
  REQUIRE(ColorTable64)
  REQUIRE(FrameInfo)
  REQUIRE(BaseColorRegionPercept)

  PROVIDE(FieldColorPercept)
END_DECLARE_MODULE(FieldColorClassifierFull)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class FieldColorParametersFull : public ParameterList
{
public:

  unsigned int maxDistY;
  unsigned int maxDistCb;
  unsigned int maxDistCr;
  unsigned int minDistY;
  unsigned int minDistCb;
  unsigned int minDistCr;
  unsigned int minCrRate;

  FieldColorParametersFull()
  :
    ParameterList("FieldColorParametersFull")
  {
      PARAMETER_REGISTER(maxDistY) = 108;
      PARAMETER_REGISTER(maxDistCb) = 5;
      PARAMETER_REGISTER(maxDistCr) = 5;
      PARAMETER_REGISTER(minDistY) = 20;
      PARAMETER_REGISTER(minDistCb) = 4;
      PARAMETER_REGISTER(minDistCr) = 4;
      PARAMETER_REGISTER(minCrRate) = 20;
  //#define MIN_FIELD_COLOR_Y_LEVEL 20
  //#define MAX_FIELD_COLOR_Y_LEVEL 224

    syncWithConfig();

    DebugParameterList::getInstance().add(this);
  }

  ~FieldColorParametersFull()
  {
    DebugParameterList::getInstance().remove(this);
  } 
}; 

class FieldColorClassifierFull : public  FieldColorClassifierFullBase
{
public:
  FieldColorClassifierFull();
  ~FieldColorClassifierFull()
  {
    outfile.close();
  }

  /** executes the module */
  void execute();

private:
  bool justStarted;
  RingBufferWithSum<int, 100> IndexYBuffer;
  RingBufferWithSum<int, 100> IndexCbBuffer;
  RingBufferWithSum<int, 100> IndexCrBuffer;
  naoth::FrameInfo lastFrame;

  FieldColorParametersFull fieldParams;

  double weightedSmoothedHistY[COLOR_CHANNEL_VALUE_COUNT];
  double weightedSmoothedHistCb[COLOR_CHANNEL_VALUE_COUNT];
  double weightedHistY[COLOR_CHANNEL_VALUE_COUNT];
  double weightedHistCb[COLOR_CHANNEL_VALUE_COUNT];
  double weightedHistCr[COLOR_CHANNEL_VALUE_COUNT];

  std::ofstream outfile;

  double smoothRungeKutta4(const unsigned int& idx, double* valueArray);

  void runDebugRequests(int weigthedMeanY, int meanY);
};

#endif // FIELDCOLORCLASSIFIERFULL_H
