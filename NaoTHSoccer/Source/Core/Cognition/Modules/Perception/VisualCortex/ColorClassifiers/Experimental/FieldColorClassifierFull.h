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
  REQUIRE(ColoredGridTop)
  REQUIRE(ColorChannelHistograms)
  REQUIRE(ColorChannelHistogramsTop)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(FrameInfo)
  REQUIRE(BaseColorRegionPercept)
  REQUIRE(BaseColorRegionPerceptTop)

  PROVIDE(FieldColorPercept)
  PROVIDE(FieldColorPerceptTop)
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
  RingBufferWithSum<int, 100> IndexYBufferTop;
  RingBufferWithSum<int, 100> IndexCbBufferTop;
  RingBufferWithSum<int, 100> IndexCrBufferTop;
  naoth::FrameInfo lastFrame;

  FieldColorParametersFull fieldParams;

  double weightedSmoothedHistY[ColorChannelHistograms::VALUE_COUNT];
  double weightedSmoothedHistCb[ColorChannelHistograms::VALUE_COUNT];
  double weightedHistY[ColorChannelHistograms::VALUE_COUNT];
  double weightedHistCb[ColorChannelHistograms::VALUE_COUNT];
  double weightedHistCr[ColorChannelHistograms::VALUE_COUNT];

  double weightedSmoothedHistTopY[ColorChannelHistograms::VALUE_COUNT];
  double weightedSmoothedHistTopCb[ColorChannelHistograms::VALUE_COUNT];
  double weightedHistTopY[ColorChannelHistograms::VALUE_COUNT];
  double weightedHistTopCb[ColorChannelHistograms::VALUE_COUNT];
  double weightedHistTopCr[ColorChannelHistograms::VALUE_COUNT];

  std::ofstream outfile;

  double smoothRungeKutta4(const unsigned int& idx, double* valueArray);

  void runDebugRequests();
};

#endif // FIELDCOLORCLASSIFIERFULL_H
