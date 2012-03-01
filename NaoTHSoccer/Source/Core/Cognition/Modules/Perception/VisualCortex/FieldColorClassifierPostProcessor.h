/* 
 * File:   FieldColorClassifierPostProcessor.h
 * Author: claas
 *
 * Created on 15. März 2011, 15:56
 */

#ifndef FIELDCOLORCLASSIFIERPOSTPROCESSOR_H
#define FIELDCOLORCLASSIFIERPOSTPROCESSOR_H

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/ColorTable64.h"
#include "Representations/Perception/FieldColorPreProcessingPercept.h"
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Infrastructure/FrameInfo.h"

// Tools
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"

//Perception
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/Histogram.h"
#include "Tools/ImageProcessing/FieldColorParameters.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(FieldColorClassifierPostProcessor)
  REQUIRE(Histogram)
  REQUIRE(Image)
  REQUIRE(FrameInfo)
  REQUIRE(FieldColorPreProcessingPercept)

  PROVIDE(FieldColorPercept)
END_DECLARE_MODULE(FieldColorClassifierPostProcessor)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class FieldColorClassifierPostProcessor : public  FieldColorClassifierPostProcessorBase
{
public:
  FieldColorClassifierPostProcessor();
  virtual ~FieldColorClassifierPostProcessor(){}

  /** executes the module */
  void execute();

private:
  double adaptationRate;
  bool enablePlots;
  bool justStarted;

  FieldColorParameters fieldParams;

  unsigned int distY;
  unsigned int distCb;
  unsigned int distCr;
  unsigned int maxDistY;
  unsigned int maxDistCb;
  unsigned int maxDistCr;

  double meanFieldY;
  double weightedMeanY;
  double maxWeightedY;
  unsigned int maxWeightedIndexY;
  double maxWeightedCb;
  unsigned int maxWeightedIndexCb;
  double maxWeightedCr;
  unsigned int maxWeightedIndexCr;

  double weightedSmoothedHistY[COLOR_CHANNEL_VALUE_COUNT];
  double weightedSmoothedHistCb[COLOR_CHANNEL_VALUE_COUNT];
  double weightedHistY[COLOR_CHANNEL_VALUE_COUNT];
  double weightedHistCb[COLOR_CHANNEL_VALUE_COUNT];

  double smoothRungeKutta4(const unsigned int& idx, double* valueArray);
  void setPerceptData();

  void runDebugRequests(int weigthedMeanY, int meanY);

};

#endif  /* FIELDCOLORCLASSIFIER_H */

