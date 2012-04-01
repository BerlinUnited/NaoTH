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
//#include "Representations/Infrastructure/ColorTable64.h"
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
  bool justStarted;

  //
  FieldColorParameters fieldParams;

  //
  double maxWeightedU;
  int indexU;
  
  double weightedHistU[COLOR_CHANNEL_VALUE_COUNT];

  int sampleCount;
  int maxSampleCount;

  double smoothRungeKutta4(const int& idx, double* valueArray) const;

};

#endif  /* FIELDCOLORCLASSIFIER_H */

