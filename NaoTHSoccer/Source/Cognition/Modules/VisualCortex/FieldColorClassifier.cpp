/*
 * File:   FieldColorClassifier.cpp
 * Author: Heinrich Mellmann
 * 
 */

#include "FieldColorClassifier.h"
#include "Tools/Math/Matrix2x2.h"

using namespace std;

FieldColorClassifier::FieldColorClassifier()
: 
  uniformGrid(getImage().width(), getImage().height(), 60, 40),
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:CamTop", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:CamBottom", "", false);

  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:markColors", "", false);

  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:histogramUV", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:histogramYCroma", "", false);

  getDebugParameterList().add(&parameters);

  for(size_t i = 0; i < CameraInfo::numOfCamera; i++) {
    histogramUVArray[i].setSize(256);
    histogramYCromaArray[i].setSize(256);
  }
}

FieldColorClassifier::~FieldColorClassifier()
{
  getDebugParameterList().remove(&parameters);
}

void FieldColorClassifier::execute(const CameraInfo::CameraID id)
{
  // TODO: set this global
  cameraID = id;
  Histogram2D& histogramUV = histogramUVArray[cameraID];
  Histogram2D& histogramYCroma = histogramYCromaArray[cameraID];
  
  double alpha = 0.99;
  // aging
  for(size_t i = 0; i < histogramUV.size(); i++) {
    for(size_t j = 0; j < histogramUV.size(); j++) {
      histogramUV(i,j) *= alpha;
      histogramYCroma(i,j) *= alpha;
    }
  }

  const int SCALE = 256 / histogramUV.size();

  Pixel pixel;
  for(unsigned int i = 0; i < uniformGrid.size(); i++)
  {
    const Vector2i& point = uniformGrid.getPoint(i);
    getImage().get(point.x, point.y, pixel);

    Vector2d dp(pixel.u - 128, pixel.v - 128);
    //double a = dp.angle();

    //if(fabs(Math::normalize(parameters.colorAngleCenter - a)) < parameters.colorAngleWith)
    {
      dp.rotate(-parameters.greenColorAngleCenter);
      int value = (int)(dp.x + 128 + 0.5);
      histogramYCroma(value/SCALE, (256 - pixel.y)/SCALE) += (1.0 - alpha);
    }

    if( !getFieldColorPercept().greenHSISeparator.noColor(pixel.y, pixel.u, pixel.v) ) {
      histogramUV(pixel.u/SCALE, pixel.v/SCALE) += (1.0 - alpha);
    }
  }


  DEBUG_REQUEST("Vision:FieldColorClassifier:markColors",
    for(unsigned int x = 0; x < getImage().width(); x+=4) {
      for(unsigned int y = 0; y < getImage().height(); y+=4) {
        getImage().get(x, y, pixel);

        if( getFieldColorPercept().greenHSISeparator.noColor(pixel.y, pixel.u, pixel.v) ) {
          POINT_PX(ColorClasses::red, x, y);
        } else if( getFieldColorPercept().greenHSISeparator.isColor(pixel.y, pixel.u, pixel.v) ) {
          POINT_PX(ColorClasses::green, x, y);
        }
      }
    }
  );

  DEBUG_REQUEST("Vision:FieldColorClassifier:histogramYCroma",
    draw_histogramUV(histogramYCroma);

    CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));

    Vector2d p0(parameters.brightnesConeRadiusBlack, 0);
    Vector2d p1(parameters.brightnesConeRadiusBlack, parameters.brightnesConeOffset);
    Vector2d p2(parameters.brightnesConeRadiusWhite, 256);

    PEN("FFFFFF99", 1);
    LINE(128 - p0.x, 256 - p0.y, 128 - p1.x, 256 - p1.y);
    LINE(128 - p1.x, 256 - p1.y, 128 - p2.x, 256 - p2.y);

    LINE(128 + p0.x, 256 - p0.y, 128 + p1.x, 256 - p1.y);
    LINE(128 + p1.x, 256 - p1.y, 128 + p2.x, 256 - p2.y);

    PEN("FFFFFF99", 1);
    LINE(128, 0, 128, 256);
  );

  DEBUG_REQUEST("Vision:FieldColorClassifier:histogramUV",
    draw_histogramUV(histogramUV);

    CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));

    Vector2d p0(128.0, 0.0);
    p0.rotate(parameters.greenColorAngleCenter);

    Vector2d p1(128.0, 0.0);
    p1.rotate(parameters.greenColorAngleCenter - parameters.greenColorAngleWith);

    Vector2d p2(128.0, 0.0);
    p2.rotate(parameters.greenColorAngleCenter + parameters.greenColorAngleWith);

    PEN("FFFFFF99", 1);
    LINE(128, 128, 128+p0.x, 128+p0.y);
    LINE(128, 128, 128+p1.x, 128+p1.y);
    LINE(128, 128, 128+p2.x, 128+p2.y);
  );

}//end execute


void FieldColorClassifier::draw_histogramUV(const Histogram2D& histUV) const
{
  double maxValueUV = 0;

  for(size_t i = 0; i < histUV.size(); i++) {
    for(size_t j = 0; j < histUV.size(); j++) {
      maxValueUV = max(maxValueUV, log(histUV(i,j)+1));
    }
  }

  if(maxValueUV == 0) {
    return;
  }

  for (size_t x = 0; x < histUV.size(); x++) {
    for (size_t y = 0; y < histUV.size(); y++)
    {
      double t = log(histUV(x,y)+1) / maxValueUV;
      getDebugImageDrawings().drawPointToImage((unsigned char)min(t*2*254, 254.0), (t > 0.1)?0:128, 128, x,y);
    }
  }
}//end draw_histogramUV

