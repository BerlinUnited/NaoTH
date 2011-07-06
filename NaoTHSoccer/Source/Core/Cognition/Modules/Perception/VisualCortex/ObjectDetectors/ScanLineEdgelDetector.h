/* 
 * File:   ScanLineEdgelDetector.h
 * Author: claas
 *
 * Created on 14. März 2011, 14:22
 */

#include "Cognition/Cognition.h"

#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/ImageProcessing/ImageDrawings.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/ImageProcessing/LineDetectorConstParameters.h"
#include "Tools/ImageProcessing/Edgel.h"

#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/ColorTable64.h"
#include "Representations/Infrastructure/CameraSettings.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"
#include "Representations/Perception/BodyContour.h"

#ifndef SCANLINEEDGELDETECTOR_H
#define	SCANLINEEDGELDETECTOR_H


BEGIN_DECLARE_MODULE(ScanLineEdgelDetector)
  REQUIRE(Image)
  REQUIRE(ColorTable64)
  REQUIRE(CurrentCameraSettings)
  REQUIRE(CameraMatrix)
  REQUIRE(FieldPercept)
  REQUIRE(FieldColorPercept)
  REQUIRE(BodyContour)

  PROVIDE(ScanLineEdgelPercept)
END_DECLARE_MODULE(ScanLineEdgelDetector)


class ScanLineState
{
public:
  unsigned int scanLineID;
  unsigned int lineEdgelCount;
  unsigned int greenCount;
  unsigned int noGreenSeen;
  unsigned int whiteCount;
  unsigned int skyblueCount;
  unsigned int yellowCount;

  double lastPixelBrightDiffs[4];

  bool greenFound;
  bool noGreenFound;

  Vector2<int> lastGreenPoint;
  Vector2<int> lastWhitePoint;
  Vector2<int> lastYellowPoint;
  Vector2<int> lastSkyBluePoint;

  ScanLineEdgelPercept::EndPoint endPoint;

  ScanLineState()
  {
    Vector2<int> start(0,0);
    reset(start, 0);
  }

  void reset(const Vector2<int>& start,unsigned int scanLineId)
  {
      greenCount = 0;
      noGreenSeen = 0;
      whiteCount = 0;
      skyblueCount = 0;
      yellowCount = 0;

      this->scanLineID = scanLineId;
      endPoint.posInImage = start;
      endPoint.color = ColorClasses::none;
      endPoint.ScanLineID = scanLineId;

      lineEdgelCount = 0;
      greenFound = false;
      noGreenFound = false;

      lastGreenPoint = start;
      lastWhitePoint = start;
      lastYellowPoint = start;
      lastSkyBluePoint = start;

      for(int i = 0; i < 4; i++)
      {
        lastPixelBrightDiffs[i] = 0.0;
      }
  }

  void addBrightDiff(double diff)
  {
    lastPixelBrightDiffs[3] = lastPixelBrightDiffs[2];
    lastPixelBrightDiffs[2] = lastPixelBrightDiffs[1];
    lastPixelBrightDiffs[1] = lastPixelBrightDiffs[0];
    lastPixelBrightDiffs[0] = diff;
  }

  double getSlope()
  {
    return lastPixelBrightDiffs[0];
    //return ((double)(lastPixelBrightDiffs[3] +  2 * (lastPixelBrightDiffs[2] + lastPixelBrightDiffs[1]) + lastPixelBrightDiffs[0])) / 6;
  }


};


class ScanLineEdgelDetector : private ScanLineEdgelDetectorBase
{
public:
  ScanLineEdgelDetector();
  ~ScanLineEdgelDetector();

  void execute();

private:

  int cameraBrighness;
  unsigned int edgelBrightnessLevel;
  unsigned int edgelGrayThresholdLevel;

  vector<ScanLineState> scanlineStates;

  void integrated_edgel_detection();
  ScanLineEdgelPercept::EndPoint scanForEdgels(int scan_id, const Vector2<int>& start, const Vector2<int>& end);

  void iterative_edgel_detection();
  Edgel getEdgel(const Vector2<int>& start, const Vector2<int>& end, ScanLineState& scanLineState);
  void getScanlineEndPoint(ScanLineState& scanLineState);

  double getPointsAngle(Vector2<int>& point);
  void evaluateEdgelThickness(Edgel& edgel);

  bool isFieldColor(const Pixel& pixel);

  double calculateMeanAngle(double angle1,double  angle2);


  void drawDebug();
};
#endif	/* SCANLINEEDGELDETECTOR_H */

