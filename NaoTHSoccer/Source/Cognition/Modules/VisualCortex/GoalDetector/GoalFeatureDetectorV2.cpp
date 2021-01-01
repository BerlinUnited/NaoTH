
/**
* @file GoalFeatureDetectorV2.cpp
*
* Implementation of class GoalFeatureDetectorV2
*
*/

#include "GoalFeatureDetectorV2.h"

#include "Tools/CameraGeometry.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/Math/Geometry.h"
#include "Tools/ImageProcessing/MaximumScan.h"

#define IMG_GET(x,y,p) \
  if(!getImage().isInside(x,y)) { \
    std::cout << __FILE__ << ":" << __LINE__ << "SCHEISSE!!!" << std::endl; \
  } \
  getImage().get(x, y, p);

#undef IMG_GET

#define IMG_GET(x,y,p) \
  getImage().get_direct(x, y, p);


GoalFeatureDetectorV2::GoalFeatureDetectorV2()
: 
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetectorV2:draw_scanlines", "..", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetectorV2:markPeaks", "mark found maximum u-v peaks in image", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetectorV2:markGradients", "mark found gradients in image", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetectorV2:draw_scanlines","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetectorV2:draw_response","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetectorV2:draw_difference","..", false);

  getGoalFeaturePercept().reset(params.numberOfScanlines);
  getGoalFeaturePerceptTop().reset(params.numberOfScanlines);

  getDebugParameterList().add(&params);
}

GoalFeatureDetectorV2::~GoalFeatureDetectorV2()
{
  getDebugParameterList().remove(&params);
}

bool GoalFeatureDetectorV2::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  // horizon
  Vector2d p1 = getArtificialHorizon().begin();
  Vector2d p2 = getArtificialHorizon().end();
  Vector2d horizonDirection = getArtificialHorizon().getDirection();

  // calculate the startpoint for the scanlines based on the horizon
  // NOTE: this point doesn't have to be at the edge of the image
  Vector2d c = (p2+p1)*0.5;
  int offsetY = params.numberOfScanlines * params.scanlinesDistance / 2 + 2;
  int y = (int)(c.y - offsetY + 0.5);
  int yc = Math::clamp(y, 0, (int)(getImage().height())-1-2*offsetY);
  Vector2d start(c.x, yc);

  // adjust the vectors if the parameters change
  if((int)getGoalFeaturePercept().features.size() != params.numberOfScanlines)
  {
    getGoalFeaturePercept().features.resize(params.numberOfScanlines);
  }

  // clear the old features
  for(size_t i = 0; i < getGoalFeaturePercept().features.size(); i++) 
  {
     getGoalFeaturePercept().features[i].clear();
  }

  ////find feature that are candidates for goal posts along scanlines 
  findEdgelFeatures(horizonDirection, start);

  return true;
}//end execute


void GoalFeatureDetectorV2::findEdgelFeatures(const Vector2d& scanDir, const Vector2i& start)
{
  const Vector2i frameUpperLeft(0,0);
  const Vector2i frameLowerRight(getImage().width()-1, getImage().height()-1);

  for(int scanId = 0; scanId < params.numberOfScanlines; scanId++)
  {
    std::vector<GoalBarFeature>& features = getGoalFeaturePercept().features[scanId];

    // adjust the start and end point for this scanline
    Vector2i pos((int) start.x, start.y + params.scanlinesDistance*scanId);
    Vector2i end;
    Math::Line scanLine(pos, scanDir);
    Geometry::getIntersectionPointsOfLineAndRectangle(frameUpperLeft, frameLowerRight, scanLine, pos, end);
    
    if(!getImage().isInside(pos.x, pos.y)) {
      continue;
    }
    
    BresenhamLineScan scanner(pos, end);

    Filter<Diff5x1, Vector2i, double, 5> filter;

    // initialize the scanner
    Vector2i peak_point(pos);
    Vector2i peak_point_max(pos);
    Vector2i peak_point_min(pos);
    MaximumScan<Vector2i,double> maxScan(peak_point_max, params.thresholdGradient);
    MaximumScan<Vector2i,double> minScan(peak_point_min, params.thresholdGradient);

    bool edgeFound = false;

    //Pixel pixel;

    EdgelD lastEdgel;

    Vector2i lastPos;
    while(scanner.getNextWithCheck(pos)) 
    {
      //IMG_GET(pos.x, pos.y, pixel);
      //int pixValue =  parameters.detectWhiteGoals ? pixel.y : (int) Math::round(((double) pixel.v - (double)pixel.u) * ((double) pixel.y / 255.0));
      int pixValue = getImage().getY_direct(pos.x, pos.y);

       DEBUG_REQUEST("Vision:GoalFeatureDetectorV2:draw_scanlines",
        POINT_PX(ColorClasses::gray, pos.x, pos.y );
      );

      if(pixValue > params.threshold)
      {
        filter.add(pos, pixValue);
      }
      else
      {
        filter.add(pos, params.threshold);
      }
      if(!filter.ready()) {
        continue;
      }

      DEBUG_REQUEST("Vision:GoalFeatureDetectorV2:draw_response",
        if(scanId == params.numberOfScanlines / 2)
        {
          int v = (int) filter.value();
          LINE_PX(ColorClasses::skyblue, lastPos.x, lastPos.y, pos.x, pos.y + v);
          if(abs(v) < params.thresholdGradient)
          {
            v = 0;
          }
          POINT_PX(ColorClasses::gray, pos.x, pos.y);
          LINE_PX(ColorClasses::red, lastPos.x, lastPos.y, pos.x, pos.y + v);
          lastPos.x = pos.x;
          lastPos.y =  pos.y + v;
        }
      );

      bool peakFound = false;
      if(maxScan.add(filter.point(), filter.value()))
      {
        peakFound = true;
        peak_point = peak_point_max;
      }
      else if(minScan.add(filter.point(), -filter.value()))
      {
        peakFound = true;
        peak_point = peak_point_min;
      }

      if(peakFound)
      {
        //Vector2d gradient = parameters.detectWhiteGoals ? calculateGradientY(peak_point) : calculateGradientUV(peak_point);
        Vector2d gradient = calculateGradientY(peak_point);
        EdgelD newEdgel;

        if(gradient.y < 0)
        {
          gradient *= -1;
          newEdgel.type = EdgelD::positive; 
        }
        else
        {
          newEdgel.type = EdgelD::negative; 
        }

         DEBUG_REQUEST("Vision:GoalFeatureDetectorV2:markGradients",
          LINE_PX(ColorClasses::black, peak_point.x, peak_point.y, peak_point.x + (int)(10*gradient.x+0.5), peak_point.y + (int)(10*gradient.y+0.5));
        );
        newEdgel.point = peak_point;
        newEdgel.direction = gradient;

        ColorClasses::Color col;
        ColorClasses::Color peakCol = ColorClasses::red;
        if(newEdgel.type == EdgelD::negative) 
        {
          col = ColorClasses::pink;
        }
        else
        {
          col = ColorClasses::orange;
          peakCol = ColorClasses::pink;
        }
        Vector2d upright = getArtificialHorizon().getDirection();
        upright.rotateRight();

       //since gradient is normalized we can just check abs(gradient.y) to sort out all not upright edges
        if(fabs(fabs(gradient.y) - fabs(upright.y)) < 0.2)
        {

          DEBUG_REQUEST("Vision:GoalFeatureDetectorV2:markGradients",
            LINE_PX(col, (int)newEdgel.point.x, (int)newEdgel.point.y, (int)newEdgel.point.x + (int)(10*newEdgel.direction.x+0.5), (int)newEdgel.point.y + (int)(10*newEdgel.direction.y+0.5));
            POINT_PX(ColorClasses::blue, (int)newEdgel.point.x, (int)newEdgel.point.y );
          );

          if(edgeFound)
          { 
            double sim = lastEdgel.sim2(newEdgel);
            if(sim > params.thresholdFeatureGradient)
            {
              GoalBarFeature feature;
              feature.point = Vector2d(lastEdgel.point + newEdgel.point) * 0.5;
          
              //IMG_GET((int)  feature.point.x, (int)  feature.point.y, pixel);
              //int pixValue =  parameters.detectWhiteGoals ? pixel.y : (int) Math::round(((double) pixel.v - (double)pixel.u) * ((double) pixel.y / 255.0));
              int pixValue = getImage().getY_direct((int)  feature.point.x, (int)  feature.point.y);
              if(pixValue > params.threshold)
              {
                double featureWidth = (newEdgel.point - lastEdgel.point).abs();
                feature.begin.point = lastEdgel.point;
                feature.begin.direction = lastEdgel.direction;
                feature.begin.type = lastEdgel.type;
                
                feature.end.point = newEdgel.point;
                feature.end.direction = newEdgel.direction;
                feature.end.type = newEdgel.type;

                feature.direction = (lastEdgel.direction + newEdgel.direction).normalize();
                feature.width = featureWidth;
                features.push_back(feature);

                DEBUG_REQUEST("Vision:GoalFeatureDetectorV2:markPeaks",
                  LINE_PX(ColorClasses::blue, (int)lastEdgel.point.x, (int)lastEdgel.point.y, (int)newEdgel.point.x,  (int)newEdgel.point.y);
                );
                DEBUG_REQUEST("Vision:GoalFeatureDetectorV2:markGradients",
                  LINE_PX(ColorClasses::green, (int) feature.point.x, (int) feature.point.y, (int) feature.point.x + (int)(10* feature.direction.x+0.5), (int) feature.point.y + (int)(10* feature.direction.y+0.5));
                  POINT_PX(ColorClasses::blue, (int) feature.point.x, (int) feature.point.y );
                );
              }
            }
          }
          edgeFound = true;
        }

        DEBUG_REQUEST("Vision:GoalFeatureDetectorV2:markPeaks",
          POINT_PX(peakCol, peak_point.x, peak_point.y );
        );
        lastEdgel.type = newEdgel.type;
        lastEdgel.direction = newEdgel.direction;
        lastEdgel.point = newEdgel.point;
      }
    }//end while
  }

}//end findfeatures


Vector2d GoalFeatureDetectorV2::calculateGradientUV(const Vector2i& point) const
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

Vector2d GoalFeatureDetectorV2::calculateGradientY(const Vector2i& point) const
{
  Vector2d gradient;
  static const int offset = 2;

  // no angle at the border (shouldn't happen)
  if( point.x < offset || point.x + offset + 1 > (int)getImage().width() ||
      point.y < offset || point.y + offset + 1 > (int)getImage().height() ) {
    return gradient;
  }

  //apply Sobel Operator on (pointX, pointY)
  //and calculate gradient in x and y direction by that means
  
  const int x0 = point.x-offset;
  const int y0 = point.y-offset;
  const int x1 = point.x+offset;
  const int y1 = point.y+offset;

  // NOTE: char type is converted to int before doing the actual calculations
  //       so we don't need cast here
  gradient.x =
       getImage().getY_direct(x0, y1)
    +2*getImage().getY_direct(point.x, y1)
    +  getImage().getY_direct(x1, y1)
    -  getImage().getY_direct(x0, y0)
    -2*getImage().getY_direct(point.x, y0)
    -  getImage().getY_direct(x1, y0);

  gradient.y =
       getImage().getY_direct(x0, y0)
    +2*getImage().getY_direct(x0, point.y)
    +  getImage().getY_direct(x0, y1)
    -  getImage().getY_direct(x1, y0)
    -2*getImage().getY_direct(x1, point.y)
    -  getImage().getY_direct(x1, y1);


  //calculate the angle of the gradient
  return gradient.normalize();
}//end calculateGradient
