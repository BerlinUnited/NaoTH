/*
 * File:   FieldColorClassifier.cpp
 * Author: claas
 * 
 * Created on 15. März 2011, 15:56
 */

#include "FieldColorClassifier.h"
#include "Tools/Math/Matrix2x2.h"

using namespace std;

FieldColorClassifier::FieldColorClassifier()
: 
  uniformGrid(getImage().width(), getImage().height(), 60, 40),
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:BottomCam:markGreen", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:TopCam:markGreen", "", false);

  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:BottomCam:histogramUV", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:TopCam:histogramUV", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:clearHistogramUV","", false);

  for(int i = 0; i < CameraInfo::numOfCamera; i++) {
    Histogram2D& histogramUV = histogramUVArray[i];
    histogramUV.resize(256);

    Histogram2D& histogramUVY = histogramUVYArray[i];
    histogramUVY.resize(256);

    for(size_t i = 0; i < histogramUV.size(); i++) {
      histogramUV[i].resize(256);
      histogramUVY[i].resize(256);
    }
  }
}

FieldColorClassifier::~FieldColorClassifier()
{

}

void FieldColorClassifier::execute(const CameraInfo::CameraID id)
{
  // TODO: set this global
  cameraID = id;

  Histogram2D& histogramUV = histogramUVArray[cameraID];
  Histogram2D& histogramUVY = histogramUVYArray[cameraID];
  

  Pixel pixel;
  const Vector2d centerUV(128,128);

  // EXPERIMENTS 2D
  DEBUG_REQUEST("Vision:FieldColorClassifier:clearHistogramUV",
    for(size_t i = 0; i < histogramUV.size(); i++) {
      for(size_t j = 0; j < histogramUV[i].size(); j++) {
        histogramUV[i][j] *= 0.99;
        histogramUVY[i][j] *= 0.99;
      }
    }
  );

  double blackOffset = 50;
  MODIFY("Vision:FieldColorClassifier:blackOffset",blackOffset);
  double brightnesConeRadiusUV = 50;
  MODIFY("Vision:FieldColorClassifier:brightnesConeRadiusUV",brightnesConeRadiusUV);


  for(unsigned int i = 0; i < uniformGrid.size(); i++)
  {
    const Vector2i& point = uniformGrid.getPoint(i);
    
    if(!getBodyContour().isOccupied(point) && point.y > getArtificialHorizon().point(point.x).y )
    {
      getImage().get(point.x, point.y, pixel);

      double yy = max(1.0,((double)pixel.y)-blackOffset) / 255.0;
      Vector2d dp = Vector2d(pixel.u, pixel.v) - centerUV;
        
      double center = -Math::pi + Math::pi_4;
      double sigma = Math::pi_4;

      if( dp.abs() > yy*brightnesConeRadiusUV ) {
        histogramUV[pixel.u][pixel.v] += 0.01;
      } else {
        histogramUVY[pixel.u][pixel.v] += 0.01;
      }
    }
  }

  DEBUG_REQUEST("Vision:FieldColorClassifier:BottomCam:histogramUV",
    if(cameraID == CameraInfo::Bottom) {
      draw_histogramUV(histogramUV, histogramUVY);
    }
  );
  DEBUG_REQUEST("Vision:FieldColorClassifier:TopCam:histogramUV",
    if(cameraID == CameraInfo::Top) {
      draw_histogramUV(histogramUV, histogramUVY);
    }
  );


  DEBUG_REQUEST("Vision:FieldColorClassifier:BottomCam:markGreen",
    if(cameraID == CameraInfo::Bottom)
    for(unsigned int x = 0; x < getImage().width(); x+=4) {
      for(unsigned int y = 0; y < getImage().height(); y+=4) {
        getImage().get(x, y, pixel);
        
        double yy = max(1.0,((double)pixel.y)-blackOffset) / 255.0;
        Vector2d dp = Vector2d(pixel.u, pixel.v) - centerUV;
        
        double center = -Math::pi + Math::pi_4;
        double sigma = Math::pi_4;

        if( dp.angle() > center - Math::pi_4 && dp.angle() < center + sigma && dp.abs() > yy*brightnesConeRadiusUV ) {
            POINT_PX(ColorClasses::red, x, y); 
        }
      }
    }
  );


  DEBUG_REQUEST("Vision:FieldColorClassifier:TopCam:markGreen",
    if(cameraID == CameraInfo::Top)
    for(unsigned int x = 0; x < getImage().width(); x+=4) {
      for(unsigned int y = 0; y < getImage().height(); y+=4) {
        getImage().get(x, y, pixel);
        
        double yy = max(1.0,((double)pixel.y)-blackOffset) / 255.0;
        Vector2d dp = Vector2d(pixel.u, pixel.v) - centerUV;
        double center = -Math::pi + Math::pi_4;
        double sigma = Math::pi_4;

        if( dp.angle() > center - Math::pi_4 && dp.angle() < center + sigma && dp.abs() > yy*brightnesConeRadiusUV ) {
            POINT_PX(ColorClasses::red, x, y); 
        }
      }
    }
  );

}//end execute


void FieldColorClassifier::draw_histogramUV(const Histogram2D& histUV, const Histogram2D& histUVY) const
{
  static const int ySize = 256;
  static const int xSize = 256;
  double yWidth = 0.5;
  double xWidth = 0.5;

  double maxValueUV = 0;
  double maxValueUVY = 0;

  for(size_t i = 0; i < histUV.size(); i++) {
    for(size_t j = 0; j < histUV[i].size(); j++) {
      maxValueUV = max(maxValueUV, log(histUV[i][j]+1));
      maxValueUVY = max(maxValueUVY, log(histUVY[i][j]+1));
    }
  }

  if(maxValueUV <= 0 || maxValueUVY <= 0) return;

  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      Vector2d point(xWidth*(2*x), yWidth*(2*y));
      
      double t = log(histUV[x][y]+1) / maxValueUV; //histogramUV[x][y];
      double s = log(histUVY[x][y]+1) / maxValueUVY;
      
      unsigned int c = 128;
      if(max(t,s) > 0 && t > s) {
        c = 0;
      }

      getDebugImageDrawings().drawPointToImage((unsigned char)(max(t,s)*254), c, c, x,y);
    }
  }
}//end draw_closest_points

