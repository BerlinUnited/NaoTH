/*
 * File:   FieldColorClassifier.cpp
 * Author: Heinrich Mellmann
 * 
 */

#include "FieldColorClassifier.h"
#include "Tools/Math/Matrix2x2.h"

using namespace std;

FieldColorClassifier::FieldColorClassifier()
: cameraID(CameraInfo::Bottom),
  uniformGrid(getImage().width(), getImage().height(), 60, 40)
{
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:CamTop", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:CamBottom", "", false);

  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:markColorsGreen", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:markColorsRed", "", false);

  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:histogramYChroma", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:histogramUVField", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:histogramUVBall", "", false);

  getDebugParameterList().add(&paramsBottom);
  getDebugParameterList().add(&paramsTop);

  for(size_t i = 0; i < CameraInfo::numOfCamera; i++) {
    histogramUVArray[i].setSize(256);
    histogramUVBallArray[i].setSize(256);
    histogramYCromaArray[i].setSize(256);
  }
}

FieldColorClassifier::~FieldColorClassifier()
{
  getDebugParameterList().remove(&paramsBottom);
  getDebugParameterList().remove(&paramsTop);
}

void FieldColorClassifier::execute(const CameraInfo::CameraID id,  Parameters& parameters)
{
  // TODO: set this global
  cameraID = id;

  // set the percept
  if(frameWhenParameterChanged.getFrameNumber() == getFrameInfo().getFrameNumber()) 
  {
    getFieldColorPercept().greenHSISeparator.set(parameters.green);
    getFieldColorPercept().redHSISeparator.set(parameters.red);

     // update cache if parameter have changed
    if(parameters.provide_colortable) {
      updateCache();
    }
  }

  DEBUG_REQUEST("Vision:FieldColorClassifier:CamBottom", if(cameraID == CameraInfo::Bottom) { debug(paramsBottom); } );
  DEBUG_REQUEST("Vision:FieldColorClassifier:CamTop", if(cameraID == CameraInfo::Top) { debug(paramsTop); } );
}

void FieldColorClassifier::debug(Parameters& parameters)
{
  Histogram2D& histogramUV = histogramUVArray[cameraID];
  Histogram2D& histogramUVBall = histogramUVBallArray[cameraID];
  Histogram2D& histogramYCroma = histogramYCromaArray[cameraID];
  
  double alpha = 0.99;
  // aging
  for(size_t i = 0; i < histogramUV.size(); i++) {
    for(size_t j = 0; j < histogramUV.size(); j++) {
      histogramUV(i,j) *= alpha;
      histogramYCroma(i,j) *= alpha;
    }
  }

  Pixel pixel;
  for(unsigned int i = 0; i < uniformGrid.size(); i++)
  {
    const Vector2i& point = uniformGrid.getPoint(i);
    getImage().get(point.x, point.y, pixel);

    Vector2d dp(pixel.u, pixel.v);
    dp.x -= 128;
    dp.y -= 128;
    //double a = dp.angle();

    //if(fabs(Math::normalize(parameters.colorAngleCenter - a)) < parameters.colorAngleWith)
    {
      dp.rotate(-parameters.green.colorAngleCenter);
      size_t value =  static_cast<size_t>(Math::clamp(dp.x + 128 + 0.5, 0.0, 255.0));
      histogramYCroma(value, (255 - pixel.y)) += (1.0 - alpha);
    }

    // collect field histogram
    if(!getFieldColorPercept().greenHSISeparator.noColor(pixel)) {
      histogramUV(pixel.u, pixel.v) += (1.0 - alpha);
    }

    // collect colored ball histogram
    if(!getFieldColorPercept().redHSISeparator.noColor(pixel)
        && !getFieldColorPercept().greenHSISeparator.noColor(pixel)
        && !getFieldColorPercept().greenHSISeparator.isChroma(pixel)) 
    {
      histogramUVBall(pixel.u, pixel.v) += (1.0 - alpha);
    }
  }


  DEBUG_REQUEST("Vision:FieldColorClassifier:markColorsGreen",
    for(unsigned int x = 0; x < getImage().width(); x+=4) {
      for(unsigned int y = 0; y < getImage().height(); y+=4) {
        getImage().get(x, y, pixel);

        if( getFieldColorPercept().greenHSISeparator.noColor(pixel.y, pixel.u, pixel.v)) {
          if (pixel.y > parameters.green.brightnesConeOffset) {
            POINT_PX(ColorClasses::red, x, y);
          } else {
            POINT_PX(ColorClasses::yellow, x, y);
          }
        } else if( getFieldColorPercept().greenHSISeparator.isColor(pixel.y, pixel.u, pixel.v)) {
          POINT_PX(ColorClasses::green, x, y);
        }
      }
    }
  );
  DEBUG_REQUEST("Vision:FieldColorClassifier:markColorsRed",
    for(unsigned int x = 0; x < getImage().width(); x+=4) {
      for(unsigned int y = 0; y < getImage().height(); y+=4) {
        getImage().get(x, y, pixel);

        if( getFieldColorPercept().redHSISeparator.noColor(pixel.y, pixel.u, pixel.v) ) {
          POINT_PX(ColorClasses::red, x, y);
        } else if( getFieldColorPercept().redHSISeparator.isColor(pixel.y, pixel.u, pixel.v) ) {
          POINT_PX(ColorClasses::blue, x, y);
        }
      }
    }
  );

  DEBUG_REQUEST("Vision:FieldColorClassifier:histogramYChroma",
    draw_histogramUV(histogramYCroma);

    CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));

    PEN("99FF9999", 1);
    draw_YChromaSeparator(parameters.green.brightnesConeOffset, parameters.green.brightnesConeRadiusBlack, parameters.green.brightnesConeRadiusWhite);

    PEN("FF999999", 1);
    draw_YChromaSeparator(parameters.red.brightnesConeOffset, parameters.red.brightnesConeRadiusBlack, parameters.red.brightnesConeRadiusWhite);
  );

  DEBUG_REQUEST("Vision:FieldColorClassifier:histogramUVField",
    draw_histogramUV(histogramUV);

    CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));

    PEN("99FF9999", 1);
    draw_UVSeparator(parameters.green.colorAngleCenter, parameters.green.colorAngleWith);
  );

  DEBUG_REQUEST("Vision:FieldColorClassifier:histogramUVBall",
    draw_histogramUV(histogramUVBall);

    CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));

    PEN("FF999999", 1);
    draw_UVSeparator(parameters.red.colorAngleCenter, parameters.red.colorAngleWith);
  );

}//end execute


void FieldColorClassifier::updateCache() {
  for (int y = 0; y < 256; y += 4) {
    for (int u = 0; u < 256; u += 4) {
      for (int v = 0; v < 256; v += 4) {
        if(getFieldColorPercept().greenHSISeparator.noColor(y, u, v)) {
          if(y > getFieldColorPercept().greenHSISeparator.getParams().brightnesConeOffset) {
            getColorTable64().setColorClass(ColorClasses::white,(unsigned char)y, (unsigned char)u, (unsigned char)v);
          } else {
            getColorTable64().setColorClass(ColorClasses::black,(unsigned char)y, (unsigned char)u, (unsigned char)v);
          }
        } else if(getFieldColorPercept().greenHSISeparator.isChroma(y, u, v)) {
          getColorTable64().setColorClass(ColorClasses::green,(unsigned char)y, (unsigned char)u, (unsigned char)v);
        }
      }
    }
  }
}

void FieldColorClassifier::draw_UVSeparator(double angleCenter, double angleWidth) const
{
  Vector2d p0(128.0, 0.0);
  p0.rotate(angleCenter);

  Vector2d p1(128.0, 0.0);
  p1.rotate(angleCenter - angleWidth);

  Vector2d p2(128.0, 0.0);
  p2.rotate(angleCenter + angleWidth);

  LINE(128, 128, 128+p0.x, 128+p0.y);
  LINE(128, 128, 128+p1.x, 128+p1.y);
  LINE(128, 128, 128+p2.x, 128+p2.y);
}

void FieldColorClassifier::draw_YChromaSeparator(double brightnesConeOffset, double brightnesConeRadiusBlack, double brightnesConeRadiusWhite) const
{
  Vector2d p0(brightnesConeRadiusBlack, 0);
  Vector2d p1(brightnesConeRadiusBlack, brightnesConeOffset);
  Vector2d p2(brightnesConeRadiusWhite, 255);

  LINE(128 - p0.x, 255 - p0.y, 128 - p1.x, 256 - p1.y);
  LINE(128 - p1.x, 255 - p1.y, 128 - p2.x, 256 - p2.y);

  LINE(128 + p0.x, 255 - p0.y, 128 + p1.x, 256 - p1.y);
  LINE(128 + p1.x, 255 - p1.y, 128 + p2.x, 256 - p2.y);

  LINE(128, 0, 128, 255);
}

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
      getDebugImageDrawings().drawPointToImage((unsigned char)min(t*2*254, 254.0), (t > 0.1)?0:128, 128, static_cast<int>(x), static_cast<int>(y));
    }
  }
}//end draw_histogramUV

