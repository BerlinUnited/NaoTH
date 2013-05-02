#ifndef COLORCALIBRATOR_H
#define COLORCALIBRATOR_H

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
#include "Tools/ImageProcessing/ColorRegion.h"
#include "Tools/ImageProcessing/CalibrationRect.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugModify.h"


class ColorCalibrator
{
public:
  ColorCalibrator(string name, ColorClasses::Color color);
  ColorCalibrator(double strength, string name, ColorClasses::Color color);
  virtual ~ColorCalibrator(){};

  /** executes the module */
  void execute(const Image& image);
  void reset();
 
  void get(PixelT<int>& idx, PixelT<int>& dist);  
  void get(colorPixel& idx, colorPixel& dist);

  void setStrength(double value)
  {
    strength = value;
  }

  void addCalibrationRect(CalibrationRect&);
  
  void drawCalibrationAreaRects();

private:
  double strength;
  string name;
  ColorClasses::Color color;

  vector<CalibrationRect*> calibrationRectangles;
  
  greenColorRegion greenParams;
  whiteColorRegion whiteParams;
  yellowColorRegion yellowParams;
  skyblueColorRegion skyblueParams;
  orangeColorRegion orangeParams;
  pinkColorRegion pinkParams;
  blueColorRegion blueParams;
  
  vector<vector<double> > histDifference;
  vector<vector<double> > histColorChannel;

  void calibrateColorRegions(const naoth::Image& image);

  void calibrateColorRegionGoal(const naoth::Image& image);

  void initHistograms
  (
    vector<vector<double> >& histColorChannel,
    vector<vector<double> >& histDifference
  );

  void getAverageDistances
  (
    const naoth::Image& image,
    ColorRegion& param
  );

  void getAverageDistances
  (
    const naoth::Image& image,
    colorPixel& ccdIdx,
    colorPixel& ccdDist,
    PixelT<int>& ccIdx,
    PixelT<int>& ccDist
  );

  void calculateRegions
  (
    colorPixel& ccdIdx,
    colorPixel& ccdDist,
    PixelT<int>& ccIdx,
    PixelT<int>& ccDist
  );

  void getAverageDistancesLoopBody
  (
    const naoth::Image& image,
    int& x,
    int& y
  );

};

#endif // COLORCALIBRATOR_H
