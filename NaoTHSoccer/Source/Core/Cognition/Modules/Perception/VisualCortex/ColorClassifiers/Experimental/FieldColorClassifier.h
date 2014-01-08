/* 
 * File:   FieldColorClassifier.h
 * Author: claas
 *
 * Created on 15. march 2011, 15:56
 */

#ifndef FIELDCOLORCLASSIFIER_H
#define FIELDCOLORCLASSIFIER_H

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/ColoredGrid.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Perception/Histograms.h"
#include "Representations/Perception/FieldColorPercept.h"

// Tools
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"

#include "Tools/ImageProcessing/ColorCalibrator.h"
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(FieldColorClassifier)
  REQUIRE(ColorChannelHistograms)
  REQUIRE(ColoredGrid)
  REQUIRE(Image)
  REQUIRE(FrameInfo)

  PROVIDE(FieldColorPercept)
END_DECLARE_MODULE(FieldColorClassifier)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class FieldColorParameters : public ParameterList
{
public:

  double CromaRedChannelDistance;
  int MaxCromaBlueChannelValue;
  int MaxBrightnessChannelValue;

  FieldColorParameters() : ParameterList("FieldColorParameters")
  {
    PARAMETER_REGISTER(MaxBrightnessChannelValue) = 120;
    PARAMETER_REGISTER(MaxCromaBlueChannelValue) = 120;
    PARAMETER_REGISTER(CromaRedChannelDistance) = 10;

    syncWithConfig();
    DebugParameterList::getInstance().add(this);
  }

  ~FieldColorParameters() {
    DebugParameterList::getInstance().remove(this);
  }
};


class FieldColorClassifier : public  FieldColorClassifierBase
{
public:
  FieldColorClassifier();
  virtual ~FieldColorClassifier(){}

  /** executes the module */
  void execute();

private:
  bool justStarted;

  FieldColorParameters fieldParams;
  CalibrationRect fieldCalibRect;
  ColorCalibrator fieldColorCalibrator;

  double maxWeightedV;
  int indexV;
  
  double weightedHistV[ColorChannelHistograms::VALUE_COUNT];
  int colorChannelHistogram[ColorChannelHistograms::VALUE_COUNT];

  double histU[ColorChannelHistograms::VALUE_COUNT];
  double histNormU[ColorChannelHistograms::VALUE_COUNT];

  int sampleCount;
  int maxSampleCount;

  double distCalib;

  void classify();
  void calibrate();
  void runDebugRequests();
};

#endif  /* FIELDCOLORCLASSIFIER_H */

