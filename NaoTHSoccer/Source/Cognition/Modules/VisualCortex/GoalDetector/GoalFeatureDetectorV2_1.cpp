
/**
* @file GoalFeatureDetectorV2_1.cpp
*
* Implementation of class GoalFeatureDetectorV2_1
*
*/

#include "GoalFeatureDetectorV2_1.h"

#include "Tools/CameraGeometry.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/Math/Geometry.h"
#include "Tools/ImageProcessing/MaximumScan.h"

#define IMG_GET(x,y,p) \
  if(!getImage().isInside(x,y)) { \
    std::cout << __FILE__ << ":" << __LINE__ << "SCHEISSE!!!" << std::endl; \
  } \
  getImage().get(x, y, p);


GoalFeatureDetectorV2_1::GoalFeatureDetectorV2_1()
: 
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetectorV2_1:draw_scanlines", "..", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetectorV2_1:markPeaks", "mark found maximum u-v peaks in image", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetectorV2_1:markGradients", "mark found gradients in image", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetectorV2_1:markWeakGradients", "mark found gradients in image", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetectorV2_1:draw_scanlines","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetectorV2_1:draw_response","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:GoalFeatureDetectorV2_1:draw_difference","..", false);

  getGoalFeaturePercept().reset(parameters.numberOfScanlines);
  getGoalFeaturePerceptTop().reset(parameters.numberOfScanlines);

  getDebugParameterList().add(&parameters);
}

GoalFeatureDetectorV2_1::~GoalFeatureDetectorV2_1()
{
  getDebugParameterList().remove(&parameters);
}

bool GoalFeatureDetectorV2_1::execute(CameraInfo::CameraID id)
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
  if((int)getGoalFeaturePercept().features.size() != parameters.numberOfScanlines) 
  {
    getGoalFeaturePercept().features.resize(parameters.numberOfScanlines);
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


void GoalFeatureDetectorV2_1::findEdgelFeatures(const Vector2d& scanDir, const Vector2i& start)
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

    Filter<Diff5x1, Vector2i, double, 5> filter;

    // initialize the scanner
    Vector2i peak_point(pos);
    Vector2i peak_point_weak(pos);
    Vector2i peak_point_max(pos);
    Vector2i peak_point_min(pos);
    Vector2i peak_point_weak_max(pos);
    Vector2i peak_point_weak_min(pos);
    MaximumScan<Vector2i,double> maxScan(peak_point_max, parameters.thresholdGradient);
    MaximumScan<Vector2i,double> minScan(peak_point_min, parameters.thresholdGradient);
    MaximumScan<Vector2i,double> maxWeakScan(peak_point_weak_max, parameters.thresholdWeakGradient);
    MaximumScan<Vector2i,double> minWeakScan(peak_point_weak_min, parameters.thresholdWeakGradient);

    bool edgeFound = false;

    Pixel pixel;

    EdgelD lastEdgel;

    std::vector<EdgelD> weakJumpPoints;
    weakJumpPoints.reserve(4);

    Vector2i lastPos;
    bool weakPeakFound = false;
    bool filterSwitchReady = false;
    while(scanner.getNextWithCheck(pos)) 
    {
      IMG_GET(pos.x, pos.y, pixel);
      int pixValue =  parameters.detectWhiteGoals ? pixel.y : (int) Math::round(((double) pixel.v - (double)pixel.u) * ((double) pixel.y / 255.0));

       DEBUG_REQUEST("Vision:GoalFeatureDetectorV2_1:draw_scanlines",
        POINT_PX(ColorClasses::gray, pos.x, pos.y );
      );

      if(pixValue > parameters.threshold)
      {
        filter.add(pos, pixValue);
      }
      else
      {
        filter.add(pos, parameters.threshold);
      }
      if(!filter.ready()) 
      {
        filterSwitchReady = true;
        continue;
      }
      bool peakFound = false;
      if(parameters.useArtificialPoints && pixValue > parameters.threshold && filterSwitchReady)
      {
        peakFound = true;
        peak_point = filter.point();
      }

      DEBUG_REQUEST("Vision:GoalFeatureDetectorV2_1:draw_response",
        if(scanId == parameters.numberOfScanlines / 2)
        {
          int v = (int) filter.value();
          LINE_PX(ColorClasses::skyblue, lastPos.x, lastPos.y, pos.x, pos.y + v);
          if(abs(v) < parameters.thresholdGradient)
          {
            v = 0;
          }
          POINT_PX(ColorClasses::gray, pos.x, pos.y);
          LINE_PX(ColorClasses::red, lastPos.x, lastPos.y, pos.x, pos.y + v);
          lastPos.x = pos.x;
          lastPos.y =  pos.y + v;
        }
      );

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

      bool edgeValid = false;
      if(peakFound)
      {
        Vector2d upright = getArtificialHorizon().getDirection();
        upright.rotateRight();

        Vector2d gradient;

        if(parameters.useArtificialPoints && filterSwitchReady)
        {
          // assume edge direction along the normal of the horizon for the artificially created jump points
          // artificial jump points are created, if the images left side is very bright (i.e. the post is only partially seen)
          gradient = upright;
          filterSwitchReady = false;
        }
        else
        {
          gradient = parameters.detectWhiteGoals ? calculateGradientY(peak_point) : calculateGradientUV(peak_point);
        }
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

         DEBUG_REQUEST("Vision:GoalFeatureDetectorV2_1:markGradients",
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

        //since gradient is normalized we can just check abs(gradient.y) to sort out all not upright edges
        if(fabs(fabs(gradient.y) - fabs(upright.y)) < 0.2)
        {

          DEBUG_REQUEST("Vision:GoalFeatureDetectorV2_1:markGradients",
            LINE_PX(col, (int)newEdgel.point.x, (int)newEdgel.point.y, (int)newEdgel.point.x + (int)(10*newEdgel.direction.x+0.5), (int)newEdgel.point.y + (int)(10*newEdgel.direction.y+0.5));
            POINT_PX(ColorClasses::blue, (int)newEdgel.point.x, (int)newEdgel.point.y );
          );

          if(edgeFound)
          { 
            double sim = lastEdgel.sim2(newEdgel);
            if(sim > parameters.thresholdFeatureGradient)
            {
              GoalBarFeature feature;
              feature.point = Vector2d(lastEdgel.point + newEdgel.point) * 0.5;
          
              IMG_GET((int)  feature.point.x, (int)  feature.point.y, pixel);
              int pixValue =  parameters.detectWhiteGoals ? pixel.y : (int) Math::round(((double) pixel.v - (double)pixel.u) * ((double) pixel.y / 255.0));
              if(pixValue > parameters.threshold)
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
                
                if(parameters.useWeakJumpsToo && weakJumpPoints.size() > 0)
                {
                  for(size_t w = 0; w < weakJumpPoints.size(); w++)
                  {
                    //add a additional feature from lastedgel to weak edgel
                    if((newEdgel.point - weakJumpPoints[w].point).abs2() > 2)
                    {
                      GoalBarFeature featureLast2Weak;
                      featureLast2Weak.point = Vector2d(lastEdgel.point + weakJumpPoints[w].point) * 0.5;
                      double featureWidth = (weakJumpPoints[w].point - lastEdgel.point).abs();
                      featureLast2Weak.begin.point = lastEdgel.point;
                      featureLast2Weak.begin.direction = lastEdgel.direction;
                      featureLast2Weak.begin.type = lastEdgel.type;
                
                      featureLast2Weak.end.point = weakJumpPoints[w].point;
                      featureLast2Weak.end.direction = weakJumpPoints[w].direction;
                      featureLast2Weak.end.type = weakJumpPoints[w].type;

                      featureLast2Weak.direction = (lastEdgel.direction + weakJumpPoints[w].direction).normalize();
                      featureLast2Weak.width = featureWidth;
                      features.push_back(featureLast2Weak);

                      //feature.weakJumps.push_back(weakJumpPoints[w]);
                      DEBUG_REQUEST("Vision:GoalFeatureDetectorV2_1:markWeakGradients",
                        LINE_PX(ColorClasses::red, (int)weakJumpPoints[w].point.x - (int)(4*weakJumpPoints[w].direction.x+0.5), (int)weakJumpPoints[w].point.y - (int)(4*weakJumpPoints[w].direction.y+0.5), (int)weakJumpPoints[w].point.x + (int)(4*weakJumpPoints[w].direction.x+0.5), (int)weakJumpPoints[w].point.y + (int)(4*weakJumpPoints[w].direction.y+0.5));
                      );
                    }
                    //add a additional feature from between weak edgels
                    if(w > 0 && (weakJumpPoints[w - 1].point - weakJumpPoints[w].point).abs2() > 2)
                    {
                      //add a feature from lastedgel to weak edgel
                      GoalBarFeature featureWeak2Weak;
                      featureWeak2Weak.point = Vector2d(weakJumpPoints[w - 1].point + weakJumpPoints[w].point) * 0.5;
                      double featureWidth = (weakJumpPoints[w].point - weakJumpPoints[w - 1].point).abs();
                      featureWeak2Weak.begin.point = weakJumpPoints[w - 1].point;
                      featureWeak2Weak.begin.direction = weakJumpPoints[w - 1].direction;
                      featureWeak2Weak.begin.type = weakJumpPoints[w - 1].type;
                
                      featureWeak2Weak.end.point = weakJumpPoints[w].point;
                      featureWeak2Weak.end.direction = weakJumpPoints[w].direction;
                      featureWeak2Weak.end.type = weakJumpPoints[w].type;

                      featureWeak2Weak.direction = (weakJumpPoints[w - 1].direction + weakJumpPoints[w].direction).normalize();
                      featureWeak2Weak.width = featureWidth;
                      features.push_back(featureWeak2Weak);
                    }
                  }
                  for(size_t w = 0; w < weakJumpPoints.size(); w++)
                  {
                    if((newEdgel.point - weakJumpPoints[w].point).abs2() > 2)
                    {
                      GoalBarFeature featureWeak2New;
                      featureWeak2New.point = Vector2d(weakJumpPoints[w].point + newEdgel.point) * 0.5;
                      double featureWidth = (newEdgel.point - weakJumpPoints[w].point).abs();
                      featureWeak2New.begin.point = weakJumpPoints[w].point;
                      featureWeak2New.begin.direction = weakJumpPoints[w].direction;
                      featureWeak2New.begin.type = weakJumpPoints[w].type;
                
                      featureWeak2New.end.point = newEdgel.point;
                      featureWeak2New.end.direction = newEdgel.direction;
                      featureWeak2New.end.type = newEdgel.type;

                      featureWeak2New.direction = (weakJumpPoints[w].direction + newEdgel.direction).normalize();
                      featureWeak2New.width = featureWidth;
                      features.push_back(featureWeak2New);
                    }
                  }
                }

                DEBUG_REQUEST("Vision:GoalFeatureDetectorV2_1:markPeaks",
                  LINE_PX(ColorClasses::blue, (int)lastEdgel.point.x, (int)lastEdgel.point.y, (int)newEdgel.point.x,  (int)newEdgel.point.y);
                );
                DEBUG_REQUEST("Vision:GoalFeatureDetectorV2_1:markGradients",
                  LINE_PX(ColorClasses::green, (int) feature.point.x, (int) feature.point.y, (int) feature.point.x + (int)(10* feature.direction.x+0.5), (int) feature.point.y + (int)(10* feature.direction.y+0.5));
                  POINT_PX(ColorClasses::blue, (int) feature.point.x, (int) feature.point.y );
                );
                edgeValid = true;
              }
            }
          }
          edgeFound = true;          
        }

        DEBUG_REQUEST("Vision:GoalFeatureDetectorV2_1:markPeaks",
          POINT_PX(peakCol, peak_point.x, peak_point.y );
        );
        lastEdgel.type = newEdgel.type;
        lastEdgel.direction = newEdgel.direction;
        lastEdgel.point = newEdgel.point;
        weakJumpPoints.clear();
      }

      if(parameters.useWeakJumpsToo)
      {
        if(!peakFound)
        {
          if(maxWeakScan.add(filter.point(), filter.value()))
          {
            weakPeakFound = true;
            peak_point_weak = peak_point_weak_max;
          }
          else if(minWeakScan.add(filter.point(), -filter.value()))
          {
            weakPeakFound = true;
            peak_point_weak = peak_point_weak_min;
          }
        }

        if(!edgeValid && weakPeakFound)
        {
          Vector2d upright = getArtificialHorizon().getDirection();
          upright.rotateRight();
          Vector2d gradientWeak = parameters.detectWhiteGoals ? calculateGradientY(peak_point_weak) : calculateGradientUV(peak_point_weak);
          if(fabs(fabs(gradientWeak.y) - fabs(upright.y)) < 0.2 && (lastEdgel.point - peak_point_weak).abs2() > 2)
          {
            EdgelD weakEdgel;

            if(gradientWeak.y < 0)
            {
              gradientWeak *= -1;
              weakEdgel.type = EdgelD::positive; 
            }
            else
            {
              weakEdgel.type = EdgelD::negative; 
            }
            weakEdgel.point = peak_point_weak;
            weakEdgel.direction = gradientWeak;
            weakJumpPoints.push_back(weakEdgel);
          }
        }
      }
      weakPeakFound = false;

    }//end while
  }

}//end findfeatures


Vector2d GoalFeatureDetectorV2_1::calculateGradientUV(const Vector2i& point) const
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

Vector2d GoalFeatureDetectorV2_1::calculateGradientY(const Vector2i& point) const
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
