
/**
* @file GoalFeatureDetector.cpp
*
* Implementation of class GoalFeatureDetector
*
*/

#include "GoalFeatureDetector.h"

#include "Tools/CameraGeometry.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/Math/Geometry.h"

#define IMG_GET(x,y,p) \
  if(!getImage().isInside(x,y)) { \
    std::cout << __FILE__ << ":" << __LINE__ << "SCHEISSE!!!" << std::endl; \
  } \
  getImage().get(x, y, p);


GoalFeatureDetector::GoalFeatureDetector()
: 
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetector:draw_scanlines", "..", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetector:markPeaks", "mark found maximum u-v peaks in image", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetector:markGradients", "mark found gradients in image", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetector:draw_scanlines","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetector:draw_response","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetector:draw_difference","..", false);

  getGoalFeaturePercept().reset(parameters.numberOfScanlines);
  getGoalFeaturePerceptTop().reset(parameters.numberOfScanlines);

  getDebugParameterList().add(&parameters);
}

GoalFeatureDetector::~GoalFeatureDetector()
{
  getDebugParameterList().remove(&parameters);
}

bool GoalFeatureDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  // horizon
  Vector2d p1 = getArtificialHorizon().begin();
  Vector2d p2 = getArtificialHorizon().end();
  Vector2d horizonDirection = getArtificialHorizon().getDirection();

  // calculate the startpoint for the scanlines based on the horizon
  // NOTE: this point doesn't have to be at the edge of the image
  Vector2d c = (p2+p1)*0.5;
  int offsetY = parameters.numberOfScanlines * parameters.scanlinesDistance / 2 + 2;
  int y = (int)(c.y - offsetY + 0.5);
  int yc = Math::clamp(y, 0, (int)(getImage().height())-1-2*offsetY);
  Vector2d start(c.x, yc);

  // adjust the vectors if the parameters change
  if((int)getGoalFeaturePercept().features.size() != parameters.numberOfScanlines) {
    getGoalFeaturePercept().features.resize(parameters.numberOfScanlines);
  }

  // clear the old features
  for(size_t i = 0; i < getGoalFeaturePercept().features.size(); i++) {
     getGoalFeaturePercept().features[i].clear();
  }

  //find feature that are candidates for goal posts along scanlines 
  if(parameters.useColorFeatures) {
    findfeaturesColor(horizonDirection, start);
  } else {
    findfeaturesDiff(horizonDirection, start);
  }

  return true;
}//end execute


void GoalFeatureDetector::findfeaturesColor(const Vector2d& scanDir, const Vector2i& start)
{
  const Vector2i frameUpperLeft(0,0);
  const Vector2i frameLowerRight(getImage().width()-1, getImage().height()-1);

  for(int scanId = 0; scanId < parameters.numberOfScanlines; scanId++)
  {
    std::vector<GoalBarFeature> & features = getGoalFeaturePercept().features[scanId];
    
    // adjust the start and end point for this scanline
    Vector2i pos((int) start.x, start.y + parameters.scanlinesDistance*scanId);
    Vector2i end;
    Math::Line scanLine(pos, scanDir);
    Geometry::getIntersectionPointsOfLineAndRectangle(frameUpperLeft, frameLowerRight, scanLine, pos, end);
    
    if(!getImage().isInside(pos.x, pos.y)) {
      continue;
    }
    
    BresenhamLineScan scanner(pos, end);
    Filter<Gauss5x1, Vector2i, double, 5> filter;
    
    Vector2i begin;
    bool begin_found = false;
    double jump = 0;

    Pixel pixel;
    while(scanner.getNextWithCheck(pos)) 
    {
      IMG_GET(pos.x, pos.y, pixel);
      int pixValue = parameters.detectWhiteGoals ? pixel.y : (int) Math::round(((double) pixel.v - (double)pixel.u) * ((double) pixel.y / 255.0));

      DEBUG_REQUEST("Vision:GoalFeatureDetector:draw_scanlines",
        POINT_PX(ColorClasses::gray, pos.x, pos.y );
      );

      filter.add(pos, pixValue);
      if(!filter.ready()) {
        continue;
      }

      if(!begin_found)
      {

        if(filter.value() > parameters.threshold)
        {
          begin = filter.point();
          DEBUG_REQUEST("Vision:GoalFeatureDetector:markPeaks",
            POINT_PX(ColorClasses::red, begin.x, begin.y );
          );
          jump = filter.value() - jump;
          begin_found = true;
        }
        else
        {
          jump = filter.value();
        }
      }

      if(begin_found && filter.value() + jump*0.5 < parameters.threshold)
      {
        const Vector2i& end = filter.point();
        DEBUG_REQUEST("Vision:GoalFeatureDetector:markPeaks",
          POINT_PX(ColorClasses::red, end.x, end.y );
        );

        Vector2d gradientBegin = parameters.detectWhiteGoals ? calculateGradientY(begin) : calculateGradientUV(begin);
        DEBUG_REQUEST("Vision:GoalFeatureDetector:markGradients", 
          LINE_PX(ColorClasses::black, begin.x, begin.y, begin.x + (int)(10*gradientBegin.x+0.5), begin.y + (int)(10*gradientBegin.y+0.5));
        );
        Vector2d gradientEnd = parameters.detectWhiteGoals ? calculateGradientY(end) : calculateGradientUV(end);
        DEBUG_REQUEST("Vision:GoalFeatureDetector:markGradients", 
          LINE_PX(ColorClasses::black, end.x, end.y, end.x + (int)(10*gradientEnd.x+0.5), end.y + (int)(10*gradientEnd.y+0.5));
        );
        int featureWidth = (end - begin).abs();
        if(fabs(gradientBegin*gradientEnd) > parameters.thresholdFeatureGradient && featureWidth < parameters.maxFeatureWidth) 
        {
          DEBUG_REQUEST("Vision:GoalFeatureDetector:markPeaks", 
            LINE_PX(ColorClasses::blue, begin.x, begin.y, end.x, end.y);
            POINT_PX(ColorClasses::red, begin.x, begin.y );
            POINT_PX(ColorClasses::red, end.x, end.y );
          );

          GoalBarFeature postFeature;
          postFeature.point = Vector2d(begin + end)*0.5;
          postFeature.direction = (gradientBegin - gradientEnd).normalize();
          //postFeature.begin = begin;
          //postFeature.end = end;
          postFeature.width = featureWidth;
          
          features.push_back(postFeature);
        }

        begin_found = false;
      }
    }//end while
  }
}//end findfeatures


void GoalFeatureDetector::findfeaturesDiff(const Vector2d& scanDir, const Vector2i& start)
{
  const Vector2i frameUpperLeft(0,0);
  const Vector2i frameLowerRight(getImage().width()-1, getImage().height()-1);

  for(int scanId = 0; scanId < parameters.numberOfScanlines; scanId++)
  {
    std::vector<GoalBarFeature>& features = getGoalFeaturePercept().features[scanId];

    // adjust the start and end point for this scanline
    Vector2i pos((int) start.x, start.y + parameters.scanlinesDistance*scanId);
    Vector2i end;
    Math::Line scanLine(pos, scanDir);
    Geometry::getIntersectionPointsOfLineAndRectangle(frameUpperLeft, frameLowerRight, scanLine, pos, end);
    
    if(!getImage().isInside(pos.x, pos.y)) {
      continue;
    }
    
    BresenhamLineScan scanner(pos, end);

    Filter<Prewitt3x1, Vector2i, double, 3> filter;
//    Filter<Diff5x1, Vector2i, double, 5> filter;

    // initialize the scanner
    Vector2i peak_point_max(pos);
    Vector2i peak_point_min(pos);
    MaximumScan<Vector2i,double> positiveScan(peak_point_max, parameters.thresholdGradient);
    MaximumScan<Vector2i,double> negativeScan(peak_point_min, parameters.thresholdGradient);

    bool begin_found = false;

    Pixel pixel;
    while(scanner.getNextWithCheck(pos)) 
    {
      IMG_GET(pos.x, pos.y, pixel);
      int pixValue =  parameters.detectWhiteGoals ? pixel.y : (int) Math::round(((double) pixel.v - (double)pixel.u) * ((double) pixel.y / 255.0));

       DEBUG_REQUEST("Vision:GoalFeatureDetector:draw_scanlines",
        POINT_PX(ColorClasses::gray, pos.x, pos.y );
      );

      filter.add(pos, pixValue);
      if(!filter.ready()) {
        continue;
      }

      if(positiveScan.add(filter.point(), filter.value()))
      {
        DEBUG_REQUEST("Vision:GoalFeatureDetector:markPeaks",
          POINT_PX(ColorClasses::red, peak_point_max.x, peak_point_max.y );
        );
        begin_found = true;
      }

      if(negativeScan.add(filter.point(), -filter.value()))
      {
        Vector2d gradientBegin = parameters.detectWhiteGoals ? calculateGradientY(peak_point_max) : calculateGradientUV(peak_point_max);
        DEBUG_REQUEST("Vision:GoalFeatureDetector:markGradients",
          LINE_PX(ColorClasses::black, peak_point_max.x, peak_point_max.y, peak_point_max.x + (int)(10*gradientBegin.x+0.5), peak_point_max.y + (int)(10*gradientBegin.y+0.5));
        );
        Vector2d gradientEnd = parameters.detectWhiteGoals ? calculateGradientY(peak_point_min) : calculateGradientUV(peak_point_min);
        DEBUG_REQUEST("Vision:GoalFeatureDetector:markGradients",
          LINE_PX(ColorClasses::black, peak_point_min.x, peak_point_min.y, peak_point_min.x + (int)(10*gradientEnd.x+0.5), peak_point_min.y + (int)(10*gradientEnd.y+0.5));
        );
        DEBUG_REQUEST("Vision:GoalFeatureDetector:markPeaks",
          POINT_PX(ColorClasses::pink, peak_point_min.x, peak_point_min.y );
        );
        // double edgel
        int featureWidth = (peak_point_max - peak_point_min).abs();
        if(begin_found && fabs(gradientBegin*gradientEnd) > parameters.thresholdFeatureGradient && featureWidth < parameters.maxFeatureWidth) 
        {
          DEBUG_REQUEST("Vision:GoalFeatureDetector:markPeaks",
            LINE_PX(ColorClasses::blue, peak_point_max.x, peak_point_max.y, peak_point_min.x, peak_point_min.y);
            POINT_PX(ColorClasses::red, peak_point_max.x, peak_point_max.y );
            POINT_PX(ColorClasses::red, peak_point_min.x, peak_point_min.y );
          );
          GoalBarFeature postFeature;
          postFeature.point = Vector2d(peak_point_max + peak_point_min)*0.5;
          postFeature.direction = (gradientBegin - gradientEnd).normalize();
          //postFeature.begin = peak_point_max;
          //postFeature.end = peak_point_min;
          postFeature.width = featureWidth;

          features.push_back(postFeature);
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

Vector2d GoalFeatureDetector::calculateGradientY(const Vector2i& point) const
{
  // no angle at the border (shouldn't happen)
  if( point.x < 2 || point.x + 3 > (int)getImage().width() ||
      point.y < 2 || point.y + 3 > (int)getImage().height() ) {
    return Vector2d();
  }

  Vector2d gradientY;
  gradientY.x =
       (int)getImage().getY(point.x-2, point.y+2)
    +2*(int)getImage().getY(point.x  , point.y+2)
    +  (int)getImage().getY(point.x+2, point.y+2)
    -  (int)getImage().getY(point.x-2, point.y-2)
    -2*(int)getImage().getY(point.x  , point.y-2)
    -  (int)getImage().getY(point.x+2, point.y-2);

  gradientY.y =
       (int)getImage().getY(point.x-2, point.y-2)
    +2*(int)getImage().getY(point.x-2, point.y  )
    +  (int)getImage().getY(point.x-2, point.y+2)
    -  (int)getImage().getY(point.x+2, point.y-2)
    -2*(int)getImage().getY(point.x+2, point.y  )
    -  (int)getImage().getY(point.x+2, point.y+2);

  return gradientY.normalize();
}
