
/**
* @file GoalFeatureDetector.cpp
*
* Implementation of class GoalFeatureDetector
*
*/

#include "GoalFeatureDetector.h"

#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugImageDrawings.h"

#include "Tools/CameraGeometry.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include <algorithm>

#define IMG_GET(x,y,p) \
  if(!getImage().isInside(x,y)) { \
    std::cout << __FILE__ << ":" << __LINE__ << "SCHEISSE!!!" << std::endl; \
  } \
  getImage().get(x, y, p);


GoalFeatureDetector::GoalFeatureDetector()
: 
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:Detectors:GoalFeatureDetector:markPeaks", "mark found maximum u-v peaks in image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:GoalFeatureDetector:draw_scanlines","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:Detectors:GoalFeatureDetector:draw_response","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:Detectors:GoalFeatureDetector:draw_difference","..", false);

  getGoalFeaturePercept().reset(params.numberOfScanlines);
  getGoalFeaturePerceptTop().reset(params.numberOfScanlines);
}


bool GoalFeatureDetector::execute(CameraInfo::CameraID id, bool horizon)
{
  cameraID = id;
  CANVAS_PX(cameraID);

  Vector2d p1(0                   , getImage().cameraInfo.getOpticalCenterY());
  Vector2d p2(getImage().width()-1, getImage().cameraInfo.getOpticalCenterY());
  Vector2d horizonDirection(1,0);

  if(!horizon && cameraID == CameraInfo::Bottom)
  {
    int offsetY = (params.numberOfScanlines * params.scanlinesDistance) / 2 + imageBorderOffset;

    p1.y = offsetY;
    p2.y = offsetY;
  }

  if(horizon) {
    p1 = getArtificialHorizon().begin();
    p2 = getArtificialHorizon().end();
    horizonDirection = getArtificialHorizon().getDirection();
  }

  // orthogonal to the horizontal scan pointing towards the ground
  getGoalFeaturePercept().horizonNormal.x = -horizonDirection.y;
  getGoalFeaturePercept().horizonNormal.y = horizonDirection.x;

  // sanity check
  if( p1.x != 0 || 
      p2.x != getImage().width()-1 ||
      p1.y < imageBorderOffset || p1.y > getImage().height() - imageBorderOffset ||
      p2.y < imageBorderOffset || p2.y > getImage().height() - imageBorderOffset
      ) 
  {
    return false;
  }
  
  int heightOfHorizon = (int) ((p1.y + p2.y) * 0.5 + 0.5);
  // image over the horizon
  if(heightOfHorizon > (int) getImage().height() - 10) {
    return false;
  }
  
  // clamp the scanline
  p1.y = Math::clamp((int) p1.y, imageBorderOffset + 5, (int)getImage().height() - imageBorderOffset - 5);
  p2.y = Math::clamp((int) p2.y, imageBorderOffset + 5 , (int)getImage().height() - imageBorderOffset - 5);
  

  // correct parameters
  // minimal 2  or more at all points have to be found
  if(params.numberOfScanlines < 2) {
    return false;
  }

  // adjust the vectors if the parameters change
  if((int)getGoalFeaturePercept().edgel_features.size() != params.numberOfScanlines) {
    getGoalFeaturePercept().edgel_features.resize(params.numberOfScanlines);
  }

  // clear the old features
  for(int i = 0; i < params.numberOfScanlines; i++) {
     getGoalFeaturePercept().edgel_features[i].clear();
  }

  //find feature that are candidates for goal posts along scanlines 
  //findfeaturesDiff(horizonDirection, p1);
  findfeaturesColor(horizonDirection, p1);

  return true;
}//end execute


void GoalFeatureDetector::findfeaturesColor(const Vector2d& scanDir, const Vector2d& p1)
{
  int offset = params.numberOfScanlines * params.scanlinesDistance / 2 + 2;
  int start = (int) p1.y - offset;
  int stop = (int) p1.y + offset;

  if(start < 0) {
    start = 2;
  }

  if(stop > (int) getImage().height()) {
    start = start - (stop - getImage().height() - 2);
  }

  int y = start;
  for(int scanId = 0; scanId < params.numberOfScanlines; scanId++)
  {
    std::vector<EdgelT<double> >& features = getGoalFeaturePercept().edgel_features[scanId];

    y += params.scanlinesDistance;
    Vector2i pos((int) p1.x + 2, y);
    Pixel pixel;
    BresenhamLineScan scanner(pos, scanDir, getImage().cameraInfo);

    Filter<Gauss,Vector2i,double> filter;
    
    Vector2i begin;
    bool begin_found = false;
    double jump = 0;

    while(scanner.getNextWithCheck(pos)) 
    {
      IMG_GET(pos.x, pos.y, pixel);
      int diffVU = (int) pixel.v - (int) pixel.u;

      filter.add(pos, diffVU);
      if(!filter.ready()) {
        continue;
      }

      if(!begin_found)
      {

        if(filter.value() > params.thresholdUV)
        {
          begin = filter.point();
          POINT_PX(ColorClasses::red, begin.x, begin.y );
          
          jump = filter.value() - jump;
          begin_found = true;
        }
        else
        {
          jump = filter.value();
        }
      }

      if(begin_found && filter.value() + jump*0.5 < params.thresholdUV)
      {
        const Vector2i& end = filter.point();
        POINT_PX(ColorClasses::red, end.x, end.y );

        Vector2d gradientBegin = calculateGradientUV(begin);
        LINE_PX(ColorClasses::black, begin.x, begin.y, begin.x + (int)(10*gradientBegin.x+0.5), begin.y + (int)(10*gradientBegin.y+0.5));

        Vector2d gradientEnd = calculateGradientUV(end);
        LINE_PX(ColorClasses::black, end.x, end.y, end.x + (int)(10*gradientEnd.x+0.5), end.y + (int)(10*gradientEnd.y+0.5));

        if(fabs(gradientBegin*gradientEnd) > params.thresholdFeatureGradient) 
        {
          LINE_PX(ColorClasses::blue, begin.x, begin.y, end.x, end.y);

          EdgelT<double> edgel;
          edgel.point = Vector2d(begin + end)*0.5;
          edgel.direction = (gradientBegin - gradientEnd).normalize();
          
          features.push_back(edgel);
        }

        begin_found = false;
      }
    }//end while
  }
}//end findfeatures


void GoalFeatureDetector::findfeaturesDiff(const Vector2d& scanDir, const Vector2d& p1)
{
  int offset = params.numberOfScanlines * params.scanlinesDistance / 2 + 2;

  int start = (int) p1.y - offset;
  int stop = (int) p1.y + offset;

  if(start < 0) {
    start = 2;
  }

  if(stop > (int) getImage().height()) {
    start = start - (stop - getImage().height() - 2);
  }

  int y = start;
  for(int scanId = 0; scanId < params.numberOfScanlines; scanId++)
  {
    std::vector<EdgelT<double> >& features = getGoalFeaturePercept().edgel_features[scanId];

    y += params.scanlinesDistance;
    Vector2i pos((int) p1.x + 2, y);
    Pixel pixel;
    BresenhamLineScan scanner(pos, scanDir, getImage().cameraInfo);

    Filter<Diff,Vector2i,double> filter;

    // initialize the scanner
    Vector2i peak_point_max(pos);
    Vector2i peak_point_min(pos);
    MaximumScan<Vector2i,double> positiveScan(peak_point_max, params.thresholdUVGradient);
    MaximumScan<Vector2i,double> negativeScan(peak_point_min, params.thresholdUVGradient);

    bool begin_found = false;

    while(scanner.getNextWithCheck(pos)) 
    {
      IMG_GET(pos.x, pos.y, pixel);
      int diffVU = (int) pixel.v - (int) pixel.u;

      filter.add(pos, diffVU);
      if(!filter.ready()) {
        continue;
      }

      if(positiveScan.add(filter.point(), filter.value()))
      {
        POINT_PX(ColorClasses::red, peak_point_max.x, peak_point_max.y );
        begin_found = true;
      }

      if(negativeScan.add(filter.point(), -filter.value()))
      {
        Vector2d gradientBegin = calculateGradientUV(peak_point_max);
        LINE_PX(ColorClasses::black, peak_point_max.x, peak_point_max.y, peak_point_max.x + (int)(10*gradientBegin.x+0.5), peak_point_max.y + (int)(10*gradientBegin.y+0.5));
        
        Vector2d gradientEnd = calculateGradientUV(peak_point_min);
        LINE_PX(ColorClasses::black, peak_point_min.x, peak_point_min.y, peak_point_min.x + (int)(10*gradientEnd.x+0.5), peak_point_min.y + (int)(10*gradientEnd.y+0.5));
        
        POINT_PX(ColorClasses::pink, peak_point_min.x, peak_point_min.y );

        // double edgel
        if(begin_found && fabs(gradientBegin*gradientEnd) > params.thresholdFeatureGradient) 
        {
          LINE_PX(ColorClasses::blue, peak_point_max.x, peak_point_max.y, peak_point_min.x, peak_point_min.y);
          POINT_PX(ColorClasses::red, peak_point_max.x, peak_point_max.y );

          EdgelT<double> edgel;
          edgel.point = Vector2d(peak_point_max + peak_point_min)*0.5;
          edgel.direction = (gradientBegin - gradientEnd).normalize();

          getGoalFeaturePercept().edgel_features[scanId].push_back(edgel);
        }

        begin_found = false;
      }

    }//end while
  }
}//end findfeatures


Vector2d GoalFeatureDetector::calculateGradientUV(const Vector2i& point) const
{
  // no angle at the border (shouldn't happen)
  if( point.x < 2 || point.x + 3 > (int)getImage().width() ||
      point.y < 2 || point.y + 3 > (int)getImage().height() ) {
    return Vector2d();
  }

  //apply Sobel Operator on (pointX, pointY)
  //and calculate gradient in x and y direction by that means
  
  Vector2d gradientU;
  gradientU.x =
       (int)getImage().getU(point.x-2, point.y+2)
    +2*(int)getImage().getU(point.x  , point.y+2)
    +  (int)getImage().getU(point.x+2, point.y+2)
    -  (int)getImage().getU(point.x-2, point.y-2)
    -2*(int)getImage().getU(point.x  , point.y-2)
    -  (int)getImage().getU(point.x+2, point.y-2);

  gradientU.y =
       (int)getImage().getU(point.x-2, point.y-2)
    +2*(int)getImage().getU(point.x-2, point.y  )
    +  (int)getImage().getU(point.x-2, point.y+2)
    -  (int)getImage().getU(point.x+2, point.y-2)
    -2*(int)getImage().getU(point.x+2, point.y  )
    -  (int)getImage().getU(point.x+2, point.y+2);


  Vector2d gradientV;
  gradientV.x =
       (int)getImage().getV(point.x-2, point.y+2)
    +2*(int)getImage().getV(point.x  , point.y+2)
    +  (int)getImage().getV(point.x+2, point.y+2)
    -  (int)getImage().getV(point.x-2, point.y-2)
    -2*(int)getImage().getV(point.x  , point.y-2)
    -  (int)getImage().getV(point.x+2, point.y-2);

  gradientV.y =
       (int)getImage().getV(point.x-2, point.y-2)
    +2*(int)getImage().getV(point.x-2, point.y  )
    +  (int)getImage().getV(point.x-2, point.y+2)
    -  (int)getImage().getV(point.x+2, point.y-2)
    -2*(int)getImage().getV(point.x+2, point.y  )
    -  (int)getImage().getV(point.x+2, point.y+2);

  //calculate the angle of the gradient
  return (gradientV - gradientU).normalize();
}
