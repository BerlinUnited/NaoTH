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

  histogramUV.resize(256);
  for(size_t i = 0; i < histogramUV.size(); i++) {
    histogramUV[i].resize(256);
  }
}

FieldColorClassifier::~FieldColorClassifier()
{

}


void FieldColorClassifier::execute(const CameraInfo::CameraID id)
{
  // TODO: set this global
  cameraID = id;
  Pixel pixel;

  // EXPERIMENTS 2D
  DEBUG_REQUEST("Vision:FieldColorClassifier:clearHistogramUV",
    for(size_t i = 0; i < histogramUV.size(); i++) {
      for(size_t j = 0; j < histogramUV[i].size(); j++) {
        histogramUV[i][j] = 0;
      }
    }
  );

  double blackOffset = 50;
  MODIFY("Vision:FieldColorClassifier:blackOffset",blackOffset);
  double brightnesConeRadiusUV = 50;
  MODIFY("Vision:FieldColorClassifier:brightnesConeRadiusUV",brightnesConeRadiusUV);
  double sigmaThresholdUV = 1;
  MODIFY("Vision:FieldColorClassifier:sigmaThresholdUV",sigmaThresholdUV);

  Moments2<2>& momentsGlobal = momentsGlobalArray[cameraID];
  
  Vector2d principleAxisMajorGlobal, principleAxisMinorGlobal;
  momentsGlobal.getAxes(principleAxisMajorGlobal, principleAxisMinorGlobal);
  Vector2d centroidGlobal = momentsGlobal.getCentroid();
  momentsGlobal.reset();

  // local moments
  Moments2<2> moments;

  for(unsigned int i = 0; i < uniformGrid.size(); i++)
  {
    const Vector2i& point = uniformGrid.getPoint(i);
    
    if(!getBodyContour().isOccupied(point) && point.y > getArtificialHorizon().point(point.x).y )
    {
      getImage().get(point.x, point.y, pixel);
      histogramUV[pixel.v][pixel.u]++;

      double gamma = 512 - pixel.v - pixel.u;
      gamma /= 512;

      if( pixel.u < 150 && pixel.v < 150 ) // NOTE: this makes problems
      {
        // cut the conus
        double yy = max(0.0,((double)pixel.y)-blackOffset) / 255.0;
        if( (Vector2d(128,128) - Vector2d(pixel.u,pixel.v)).abs() > yy*brightnesConeRadiusUV && pixel.y > blackOffset) 
        {
          Vector2i p(pixel.v, pixel.u);
          momentsGlobal.add(p, gamma);

          if((Vector2d(p) - centroidGlobal).abs() < 2*principleAxisMajorGlobal.abs()) {
            moments.add(p);
          }
        }
      }
    }
  }


  Vector2d principleAxisMajor, principleAxisMinor;
  moments.getAxes(principleAxisMajor, principleAxisMinor);
  Vector2d centroid = moments.getCentroid();

  CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
  PEN("FF0000", 1);
  OVAL_ROTATED(centroid.x, 
               centroid.y, 
               principleAxisMinor.abs(),
               principleAxisMajor.abs(),
               principleAxisMinor.angle());



  DEBUG_REQUEST("Vision:FieldColorClassifier:BottomCam:histogramUV",
    if(cameraID == CameraInfo::Bottom) draw_histogramUV();
  );
  DEBUG_REQUEST("Vision:FieldColorClassifier:TopCam:histogramUV",
    if(cameraID == CameraInfo::Top) draw_histogramUV();
  );

  // UV eigenspace transormation matrix
  Matrix2d mx(principleAxisMajor, principleAxisMinor);
  if(fabs(mx.det()) < 1e-12) return;
  mx = mx.invert();


  DEBUG_REQUEST("Vision:FieldColorClassifier:BottomCam:markGreen",
    if(cameraID == CameraInfo::Bottom)
    for(unsigned int x = 0; x < getImage().width(); x+=4) {
      for(unsigned int y = 0; y < getImage().height(); y+=4) {
        getImage().get(x, y, pixel);
        
        Vector2d diff = Vector2d(pixel.v, pixel.u) - centroid;
        double yy = max(0.0,((double)pixel.y)-blackOffset) / 255.0;

        if( (Vector2d(128,128) - Vector2d(pixel.u,pixel.v)).abs() > yy*brightnesConeRadiusUV && pixel.y > blackOffset && (mx*diff).abs() < sigmaThresholdUV ) { //(mx*diff).abs() < sigmaThreshold ) {
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
        
        Vector2d diff = Vector2d(pixel.v, pixel.u) - centroid;
        double yy = max(0.0,((double)pixel.y)-blackOffset) / 255.0;

        if( (Vector2d(128,128) - Vector2d(pixel.u,pixel.v)).abs() > yy*brightnesConeRadiusUV && pixel.y > blackOffset && (mx*diff).abs() < sigmaThresholdUV ) { //(mx*diff).abs() < sigmaThreshold ) {
          POINT_PX(ColorClasses::red, x, y); 
        }
      }
    }
  );

}//end execute


void FieldColorClassifier::draw_histogramUV() const
{
  static const int ySize = 256;
  static const int xSize = 256;
  double yWidth = 0.5;
  double xWidth = 0.5;

  FIELD_DRAWING_CONTEXT;
  Color white(1.0,1.0,1.0);
  Color black(0.0,0.0,0.0);

  double maxValue = 0;
  for(size_t i = 0; i < histogramUV.size(); i++) {
    for(size_t j = 0; j < histogramUV[i].size(); j++) {
      double l = histogramUV[i][j];//log(histogramUV[i][j]+1);
      maxValue = max(maxValue, l);
    }
  }

  if(maxValue <= 0) return;

  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      Vector2d point(xWidth*(2*x), yWidth*(2*y));
      
      double t = /*log(histogramUV[x][y]+1)*/ histogramUV[x][y] / maxValue;
      Color color = black*t + white*(1-t);
      //PEN(color, 20);
      //FILLBOX(point.x - xWidth, point.y - yWidth, point.x+xWidth, point.y+yWidth);
      getDebugImageDrawings().drawPointToImage((unsigned char)(t*254), 128, 128, x,y);
    }
  }
}//end draw_closest_points

