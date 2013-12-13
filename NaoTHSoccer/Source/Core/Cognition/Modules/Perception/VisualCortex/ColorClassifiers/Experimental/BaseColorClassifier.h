#ifndef BASECOLORCLASSIFIER_H
#define BASECOLORCLASSIFIER_H

#include <ModuleFramework/Module.h>

//Parameters
#include "BaseColorRegionParameters.h"

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
#include "Tools/DataStructures/Histogram.h"
#include <Tools/Debug/DebugParameterList.h>

//Perception
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/Histograms.h"
#include "Tools/ImageProcessing/ColorCalibrator.h"
//#include "Tools/ImageProcessing/CameraParamCorrection.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(BaseColorClassifier)
  REQUIRE(ColoredGrid)
  REQUIRE(ColoredGridTop)
  REQUIRE(ColorChannelHistograms)
  REQUIRE(ColorChannelHistogramsTop)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(FrameInfo)
  REQUIRE(GoalPercept)
  REQUIRE(GoalPerceptTop)

  //PROVIDE(FieldColorPercept)
  //PROVIDE(FieldColorPerceptTop)
  PROVIDE(BaseColorRegionPercept)
  PROVIDE(BaseColorRegionPerceptTop)
END_DECLARE_MODULE(BaseColorClassifier)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class BaseColorClassifierParameters : public ParameterList
{
public:

  double calibrationStrength;

  BaseColorClassifierParameters()
  : 
    ParameterList("BCC_Parameters")
  {
    PARAMETER_REGISTER(calibrationStrength) = 1.8;        

    syncWithConfig();

    DebugParameterList::getInstance().add(this);
  }

  ~BaseColorClassifierParameters()
  {
    DebugParameterList::getInstance().remove(this);
  } 

};

class BaseColorClassifier : public  BaseColorClassifierBase
{
public:
  BaseColorClassifier();
  virtual ~BaseColorClassifier(){}

  /** executes the module */
  void execute();

private:
  double adaptationRate;

  RingBufferWithSum<double, 100> meanY;
  RingBufferWithSum<double, 100> meanU;
  RingBufferWithSum<double, 100> meanV;
  RingBufferWithSum<double, 100> meanTopY;
  RingBufferWithSum<double, 100> meanTopU;
  RingBufferWithSum<double, 100> meanTopV;

  RingBufferWithSum<double, 100> minY;
  RingBufferWithSum<double, 100> maxY;
  RingBufferWithSum<double, 100> minTopY;
  RingBufferWithSum<double, 100> maxTopY;
  RingBufferWithSum<double, 100> spanWidthY;
  RingBufferWithSum<double, 100> spanWidthTopY;

  RingBufferWithSum<double, 100> goalMeanY;
  RingBufferWithSum<double, 100> goalMeanU;
  RingBufferWithSum<double, 100> goalMeanV;

  bool goalIsCalibrating;

  double lastMeanY;
  double lastMeanU;
  double lastMeanV;
  double lastMeanTopY;
  double lastMeanTopU;
  double lastMeanTopV;

  double lastMinY;
  double lastMaxY;
  double lastMinTopY;
  double lastMaxTopY;

  double lastSpanWidthY;
  double lastSpanWidthTopY;

  FrameInfo lastFrameInfo;

  void initPercepts();

  void setColorRegions();

  void calibrate();
  void calibrateColorRegions();

  void runDebugRequests();

  BaseColorClassifierParameters params;

  BaseColorRegionParameters regionParams;

  unsigned int fieldHist[3][ColorChannelHistograms::VALUE_COUNT];
  double fieldCalibHist[2][ColorChannelHistograms::VALUE_COUNT];
  double fieldWeightedHist[3][ColorChannelHistograms::VALUE_COUNT];
  double fieldCalibMeanY;
  double fieldCalibMeanCountY;

  int calibCount;
  bool calibrating;

  RingBufferWithSum<int, 100> sampleMinDistVu;
  RingBufferWithSum<int, 100> sampleMaxDistVu;

  Pixel minYellow;
  Pixel maxYellow;

  PixelT<int> maxWeightedIndex;
  PixelT<double> maxWeighted;
  PixelT<double> weighted;

  PixelT<int> distField;

  int minDistVminusU;
  int maxDistVminusU;

  Pixel minEnv;
  Pixel maxEnv;

  CalibrationRect orangeBallCalibRectTop;
  CalibrationRect orangeBallCalibRect;
  CalibrationRect yellowGoalPostLeftCalibRectTop;
  CalibrationRect yellowGoalPostLeftCalibRect;
  CalibrationRect yellowGoalPostRightCalibRectTop;
  CalibrationRect yellowGoalPostRightCalibRect;
  CalibrationRect blueGoalLeftCalibRectTop;
  CalibrationRect blueGoalLeftCalibRect;
  CalibrationRect blueGoalRightCalibRectTop;
  CalibrationRect blueGoalRightCalibRect;
  CalibrationRect blueTeamCalibRectTop;
  CalibrationRect blueTeamCalibRect;
  CalibrationRect redTeamCalibRectTop;
  CalibrationRect redTeamCalibRect;
  CalibrationRect whiteLinesCalibRectTop;
  CalibrationRect whiteLinesCalibRect;

  ColorCalibrator orangeBallColorCalibratorTop;
  ColorCalibrator orangeBallColorCalibrator;
  ColorCalibrator yellowGoalColorCalibratorTop;
  ColorCalibrator yellowGoalColorCalibrator;
  ColorCalibrator blueGoalColorCalibratorTop;
  ColorCalibrator blueGoalColorCalibrator;
  ColorCalibrator blueTeamColorCalibratorTop;
  ColorCalibrator blueTeamColorCalibrator;
  ColorCalibrator redTeamColorCalibratorTop;
  ColorCalibrator redTeamColorCalibrator;
  ColorCalibrator whiteLinesColorCalibratorTop;
  ColorCalibrator whiteLinesColorCalibrator;

};

#endif // BASECOLORCLASSIFIER_H
