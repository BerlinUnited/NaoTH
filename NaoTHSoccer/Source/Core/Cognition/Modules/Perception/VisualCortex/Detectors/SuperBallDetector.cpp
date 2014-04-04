
/**
* @file SuperBallDetector.cpp
*
* Implementation of class SuperBallDetector
*
*/

#include "SuperBallDetector.h"

#include "Tools/DataStructures/ArrayQueue.h"
#include "Tools/CameraGeometry.h"

#include "Tools/Debug/Stopwatch.h"

#include <Representations/Infrastructure/CameraInfoConstants.h>
#include "Tools/DoubleCamHelpers.h"

#include "Tools/Math/Geometry.h"

SuperBallDetector::SuperBallDetector()
{
  DEBUG_REQUEST_REGISTER("Vision:Detectors:SuperBallDetector:markPeakScanFull", "mark the scanned points in image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:SuperBallDetector:markPeakScan", "mark the scanned points in image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:SuperBallDetector:markPeak", "mark found maximum red peak in image", false);


  DEBUG_REQUEST_REGISTER("Vision:Detectors:SuperBallDetector:draw_ball","..", false);  
}


void SuperBallDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);

  getBallPercept().reset();


  // STEP 1: find the starting point for the search
	Vector2i start;
	if(!findMaximumRedPoint(start)) {
    return;
  }

  Pixel pixel;
  getImage().get(start.x, start.y, pixel);
  unsigned char b = pixel.b;
  unsigned char c = pixel.c;

  // ball color
  bool ballFound = false;

  double d = (Math::sqr((255.0 - (double)b)) + Math::sqr((double)c)) / (2.0*255.0);
  unsigned char t = (unsigned char)Math::clamp(Math::round(d),0.0,255.0);

  static int numberOfUpdates = 0;

  if(t > params.orange_thresh)
  {
    DEBUG_REQUEST("Vision:Detectors:SuperBallDetector:draw_ball",
      CIRCLE_PX(ColorClasses::yellow, start.x, start.y, 10);
    );

    filteredHistogramY.add(pixel.y);
    filteredHistogramU.add(pixel.u);
    filteredHistogramV.add(pixel.v);

    filteredHistogramY.calculate();
    filteredHistogramU.calculate();
    filteredHistogramV.calculate();
    numberOfUpdates++;

    ballFound = true;
  }

  filteredHistogramY.plotNormalized("filteredHistogramY");
  filteredHistogramU.plotNormalized("filteredHistogramU");
  filteredHistogramV.plotNormalized("filteredHistogramV");

  if(numberOfUpdates > 60) {
    if( pixel.y + filteredHistogramY.sigma*params.sigmaFactorY >= filteredHistogramY.mean &&
        pixel.y <= filteredHistogramY.mean + filteredHistogramY.sigma*params.sigmaFactorY &&

        pixel.u + filteredHistogramU.sigma*params.sigmaFactorUV >= filteredHistogramU.max &&
        pixel.u <= filteredHistogramU.max + filteredHistogramU.sigma*params.sigmaFactorUV &&

        pixel.v + filteredHistogramV.sigma*params.sigmaFactorUV >= filteredHistogramV.max &&
        pixel.v <= filteredHistogramV.max + filteredHistogramV.sigma*params.sigmaFactorUV)
    {

      DEBUG_REQUEST("Vision:Detectors:SuperBallDetector:draw_ball",
        CIRCLE_PX(ColorClasses::red, start.x, start.y, 10);
      );

      ballFound = true;
    }
  }


  if(ballFound)
  {
    // calculate the ball
    bool projectionOK = CameraGeometry::imagePixelToFieldCoord(
		  getCameraMatrix(), 
		  getImage().cameraInfo,
		  start.x, 
		  start.y, 
		  getFieldInfo().ballRadius,
		  getBallPercept().bearingBasedOffsetOnField);

    // HACK: don't take to far balls
    projectionOK = projectionOK && getBallPercept().bearingBasedOffsetOnField.abs2() < 10000*10000; // closer than 10m

    // HACK: if the ball center is in image it has to be in the field polygon 
    Vector2d ballPointToCheck(start.x, start.y - 5);
    projectionOK = projectionOK && 
      (getImage().isInside((int)(ballPointToCheck.x+0.5), (int)(ballPointToCheck.y+0.5)) && 
        getFieldPercept().getValidField().isInside(ballPointToCheck));


    if(projectionOK) 
    {
		  getBallPercept().radiusInImage = 5;
		  getBallPercept().centerInImage = start;
		  getBallPercept().ballWasSeen = projectionOK;
		  getBallPercept().frameInfoWhenBallWasSeen = getFrameInfo();
		
      DEBUG_REQUEST("Vision:Detectors:SuperBallDetector:draw_ball",
        CIRCLE_PX(ColorClasses::orange, start.x, start.y, 10);
		  );
    }
  }

  PLOT("SuperBallDetector:ballFound",ballFound);
}


bool SuperBallDetector::findMaximumRedPoint(Vector2i& peakPos) const
{
  //
  // STEP I: find the maximal height minY to be scanned in the image
  //
  if(!getFieldPercept().valid) {
    return false;
  }

  const FieldPercept::FieldPoly& fieldPolygon = getFieldPercept().getValidField();

  // find the top point of the polygon
  int minY = getImage().height();
  for(int i = 0; i < fieldPolygon.length ; i++)
  {
    if(fieldPolygon.points[i].y < minY && fieldPolygon.points[i].y >= 0) {
      minY = fieldPolygon.points[i].y;
    }
  }

  // double check: polygon is empty
  if(minY == (int)getImage().height() || minY < 0) {
    return false;
  }


  //
  // STEP II: calculate the step size for the scan
  //
  int stepSizeAdjusted = params.stepSize;    
  if(getCameraMatrix().rotation.getYAngle() > Math::fromDegrees(40))
  {
    stepSizeAdjusted *= 3;
  }
  else if(getCameraMatrix().rotation.getYAngle() > Math::fromDegrees(10))
  {
    stepSizeAdjusted *= 2;
  }



  int maxRedPeak = getFieldColorPercept().range.getMax().v; // initialize with the maximal red croma of the field color
  Vector2i point;
  Pixel pixel;

  for(point.y = minY; point.y < (int) getImage().height() - 3; point.y += stepSizeAdjusted) {
    for(point.x = 0; point.x < (int) getImage().width(); point.x += stepSizeAdjusted)
    {
      getImage().get(point.x, point.y, pixel);
     
      DEBUG_REQUEST("Vision:Detectors:SuperBallDetector:markPeakScanFull",
        POINT_PX(ColorClasses::blue, point.x, point.y);
      );

      if
      (
        maxRedPeak < pixel.v && // "v" is the croma RED channel
        pixel.y +  params.minOffsetToFieldY > getFieldColorPercept().histogramField.y && // brighter than darkest acceptable green
        fieldPolygon.isInside_inline(point) // only points inside the field polygon
      )
      {
        maxRedPeak = pixel.v;
        peakPos = point;

        DEBUG_REQUEST("Vision:Detectors:SuperBallDetector:markPeakScan",
          POINT_PX(ColorClasses::red, point.x, point.y);
        );
      }
    }
  }

  DEBUG_REQUEST("Vision:Detectors:SuperBallDetector:markPeak",
    LINE_PX(ColorClasses::skyblue, peakPos.x-10, peakPos.y, peakPos.x+10, peakPos.y);
    LINE_PX(ColorClasses::skyblue, peakPos.x, peakPos.y-10, peakPos.x, peakPos.y+10);
  );

  return maxRedPeak >= 0;
}
