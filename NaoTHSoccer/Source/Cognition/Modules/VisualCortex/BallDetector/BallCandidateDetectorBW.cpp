
/**
* @file BallCandidateDetectorBW.cpp
*
* Implementation of class BallCandidateDetectorBW
*
*/

#include "BallCandidateDetectorBW.h"

#include "Tools/DataStructures/ArrayQueue.h"
#include "Tools/CameraGeometry.h"

#include <Representations/Infrastructure/CameraInfoConstants.h>

#include "Tools/Math/Geometry.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/ImageProcessing/MaximumScan.h"
#include "Tools/ImageProcessing/Filter.h"

BallCandidateDetectorBW::BallCandidateDetectorBW()
{
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetectorBW:peak_scan:mark_full", "mark the scanned points in image", false);

  for(size_t x = 0; x < getImage().width()/FACTOR; ++x) {
    for(size_t y = 0; y < getImage().height()/FACTOR; ++y) {
      integralImage[x][y] = 0;
    }
  }
}

BallCandidateDetectorBW::~BallCandidateDetectorBW()
{
}

void BallCandidateDetectorBW::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  integralBild();
  
  Vector2i center;
  int valueMax = 0;
  int radius = 0;

  Vector2i point;

  int radiusEstimation[480];

  point.x = 320;
  for(point.y = 0; point.y < (int)getImage().height(); point.y++){
    radiusEstimation[point.y] = (int)(estimatedBallRadius(point)+0.5)*2;
    //CIRCLE_PX(ColorClasses::orange, point.x, point.y, r);
  }

  for(point.y = 0; point.y < (int)getImage().height()/FACTOR; point.y+=1) 
  {
    int r = radiusEstimation[point.y*FACTOR]*2/FACTOR;
    int so = (int)(r*0.3+0.5);

    if (point.y < so || point.y+r+so+1 > 480/FACTOR) {
      continue;
    }

    for(point.x = so; point.x + r + so < (int)getImage().width()/FACTOR; point.x+=1) 
    {
      int v  = integralImage[point.x+r][point.y+r]      +integralImage[point.x][point.y]      -integralImage[point.x][point.y+r]      -integralImage[point.x+r][point.y];
      int vo = integralImage[point.x+r+so][point.y+r+so]+integralImage[point.x-so][point.y-so]-integralImage[point.x-so][point.y+r+so]-integralImage[point.x+r+so][point.y-so];
      int vx = v - 2*(vo - v);

      if ( vx > valueMax ) {
        valueMax = vx;
        center.x = (point.x+r/2+1)*FACTOR;
        center.y = (point.y+r/2+1)*FACTOR;
        radius = radiusEstimation[point.y*FACTOR];
      }
    }
  }
  
  /*
  for(point.y = 0; point.y < (int)getImage().height()/4; point.y+=1) 
  {
    int r = radiusEstimation[point.y*4]*2/4;
    int so = (int)(r*2.0*0.3/4.0+0.5);

    if (point.y - so < 0 || point.y+r+so+1 > 480/4) {
      continue;
    }

    for(point.x = so; point.x < (int)getImage().width()/4 - r - so; point.x+=1) 
    {

      int v  = integralImage[point.x+r][point.y+r]      +integralImage[point.x][point.y]      -integralImage[point.x][point.y+r]      -integralImage[point.x+r][point.y];
      int vo = integralImage[point.x+r+so][point.y+r+so]+integralImage[point.x-so][point.y-so]-integralImage[point.x-so][point.y+r+so]-integralImage[point.x+r+so][point.y-so];
      int vx = v - (vo - v);

      int vi = (int)(((double)vx)/((double)valueMax)*255);
      getDebugImageDrawings().drawPointToImage(v,0,0,point.x*4,point.y*4);
    }
  }
  */

  CIRCLE_PX(ColorClasses::orange, center.x, center.y, radius);
}

double BallCandidateDetectorBW::estimatedBallRadius(const Vector2i& point) const
{
  double ballRadius = 40.0;
  Vector2d pointOnField;
  if(!CameraGeometry::imagePixelToFieldCoord(
		  getCameraMatrix(), 
		  getImage().cameraInfo,
		  point.x, 
		  point.y, 
		  ballRadius,
		  pointOnField))
  {
    return -1;
  }

  /*
  Vector3d upf(pointOnField.x, pointOnField.y, ballRadius*2);
  Vector2d up;
  if(!CameraGeometry::relativePointToImage(
        getCameraMatrix(), 
		    getImage().cameraInfo,
        upf,
        up
    ))
  {
    return -1;
  }

  Vector3d lof(pointOnField.x, pointOnField.y, 0);

  Vector2d lo;
  if(!CameraGeometry::relativePointToImage(
        getCameraMatrix(), 
		    getImage().cameraInfo,
        lof,
        lo
    ))
  {
    return -1;
  }

  return (up-lo).abs()/2.0;
  */

  Vector3d d = getCameraMatrix().invert()*Vector3d(pointOnField.x, pointOnField.y, ballRadius);
  double cameraBallDistance = d.abs();
  if(cameraBallDistance > ballRadius) {
    double a = atan2(ballRadius, d.abs());
    return a / getImage().cameraInfo.getOpeningAngleHeight() * getImage().cameraInfo.resolutionHeight;
  }
  
  return -1;
}

void BallCandidateDetectorBW::integralBild()
{
  Pixel pixel;
  Vector2i point;
  for(point.x = 1; point.x < (int)getImage().width()/FACTOR; ++point.x) 
  {
    for(point.y = 1; point.y < (int)getImage().height()/FACTOR; ++point.y) {
      getImage().get(point.x*FACTOR, point.y*FACTOR, pixel);
      
      integralImage[point.x][point.y] = integralImage[point.x-1][point.y] + integralImage[point.x][point.y-1] - integralImage[point.x-1][point.y-1];
      if(pixel.y > 40 && getFieldColorPercept().greenHSISeparator.noColor(pixel) 
                      && !getBodyContour().isOccupied(point)) {
        integralImage[point.x][point.y] += pixel.y;
      }
    }
  }

  /*
  for(size_t x = 1; x < getImage().width(); ++x) {
    for(size_t y = 1; y < getImage().height(); ++y) {
      int v = (int)(((double)integralImage[x][y])/((double)integralImage[getImage().width()-1][getImage().height()-1])*255);
      getDebugImageDrawings().drawPointToImage(v,0,0,x,y);
    }
  }
  */
}
