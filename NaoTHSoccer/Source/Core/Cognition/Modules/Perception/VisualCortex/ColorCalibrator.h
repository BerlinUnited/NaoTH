#ifndef BASECOLORCALIBRATOR_H
#define BASECOLORCALIBRATOR_H

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
//#include "Tools/ImageProcessing/BaseColorRegionParameters.h"
#include "Tools/ImageProcessing/ColorRegion.h"
#include "Tools/ImageProcessing/CalibrationRect.h"
//#include "Tools/ImageProcessing/CameraParamCorrection.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugModify.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(ColorCalibrator)
  REQUIRE(ColoredGrid)
  REQUIRE(Histogram)
  REQUIRE(Image)
  REQUIRE(FrameInfo)
  REQUIRE(FieldColorPercept)
  REQUIRE(GoalPercept)

  PROVIDE(BaseColorRegionPercept)
  PROVIDE(ColorTable64)
END_DECLARE_MODULE(ColorCalibrator)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class ColorCalibrator : public  ColorCalibratorBase
{
public:
  ColorCalibrator();
  virtual ~ColorCalibrator(){}

  /** executes the module */
  void execute();

private: 
  CalibrationRect leftGoalPost;
  CalibrationRect rightGoalPost;

  CalibrationRect orangeBall;

  CalibrationRect pinkWaistBand;
  CalibrationRect blueWaistBand;

  CalibrationRect leftField;
  CalibrationRect rightField;
  
  greenColorRegion fieldParams;
  whiteColorRegion linesParams;
  yellowColorRegion yellowGoalParams;
  skyblueColorRegion blueGoalParams;
  orangeColorRegion orangeBallParams;
  pinkColorRegion pinkWaistBandParams;
  blueColorRegion blueWaistBandParams;

  vector<vector<double> > ballHistDifference;
  vector<vector<double> > ballHistColorChannel;
  vector<vector<double> > goalHistDifference;
  vector<vector<double> > goalHistColorChannel;
  vector<vector<double> > pinkWaistBandHistDifference;
  vector<vector<double> > pinkWaistBandHistColorChannel;
  vector<vector<double> > blueWaistBandHistDifference;
  vector<vector<double> > blueWaistBandHistColorChannel;
  vector<vector<double> > fieldHistDifference;
  vector<vector<double> > fieldHistColorChannel;

  void calibrateColorRegions();

  void calibrateColorRegionField();
  void calibrateColorRegionBall();
  void calibrateColorRegionGoal();
  void calibrateColorRegionPinkWaistBand();
  void calibrateColorRegionBlueWaistBand();

  void  ColorCalibrator::initHistograms
  (
    vector<vector<double> >& histColorChannel,
    vector<vector<double> >& histDifference
  );

  void getAverageDistances
  (
    CalibrationRect& cRect,
    vector<vector<double> >& histColorChannel,
    vector<vector<double> >& histDifference,
    ColorRegion& param
  );

  void getAverageDistances2
  (
    CalibrationRect& cRect1,
    CalibrationRect& cRect2,
    vector<vector<double> >& histColorChannel,
    vector<vector<double> >& histDifference,
    ColorRegion& param
  );

  void getAverageDistances2
  (
    CalibrationRect& cRect1,
    CalibrationRect& cRect2,
    vector<vector<double> >& histColorChannel,
    vector<vector<double> >& histDifference,
    colorPixel& ccdIdx,
    colorPixel& ccdDist,
    PixelT<int>& ccIdx,
    PixelT<int>& ccDist
  );

  void calculateRegions
  (
    vector<vector<double> >& histColorChannel,
    vector<vector<double> >& histDifference,
    colorPixel& ccdIdx,
    colorPixel& ccdDist,
    PixelT<int>& ccIdx,
    PixelT<int>& ccDist
  );

  void getAverageDistancesLoopBody
  (
    int& x,
    int& y,
    vector<vector<double> >& histColorChannel,
    vector<vector<double> >& histDifference
  );

  void runDebugRequests();

  //BaseColorRegionParameters regionParams;

};

#endif // BASECOLORCALIBRATOR_H
