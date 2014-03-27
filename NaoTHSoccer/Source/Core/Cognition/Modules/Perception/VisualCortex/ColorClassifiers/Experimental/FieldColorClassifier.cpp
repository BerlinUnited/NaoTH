/*
 * File:   FieldColorClassifier.cpp
 * Author: claas
 *
 * Created on 15. march 2011, 15:56
 */

#include "FieldColorClassifier.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugImageDrawings.h"

using namespace std;
// Debug


FieldColorClassifier::FieldColorClassifier()
{
  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:FieldColorClassifier:set_dev_V1", " ", false);
  //DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:FieldColorClassifier:set_dev_V2", " ", false);
  //DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:FieldColorClassifier:set_dev_U", " ", false);
  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:FieldColorClassifier:set_dev_Y1", " ", false);

  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:FieldColorClassifier:set_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:FieldColorClassifier:enable_plot", " ", false);

  histY.setMaxTotalSum(12000 * getColoredGrid().uniformGrid.size());
  histU.setMaxTotalSum(12000 * getColoredGrid().uniformGrid.size());
  histV.setMaxTotalSum(12000 * getColoredGrid().uniformGrid.size());
}

void FieldColorClassifier::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);

#ifdef QVGA
  if(cameraID == CameraInfo::Bottom)
#endif
  {
    Pixel pixel;

    //histY_1.clear();
    //histV_1.clear();
    //histV_2.clear();
    //histY.clear();
    //histU.clear();

    const Statistics::HistogramX& histogramY = getColorChannelHistograms().histogramY;
    double start = histogramY.min;
    double end = histogramY.max;
    double span = histogramY.spanWidth;
    double halfSpan = span / 2.0;
    double quadSpan = span / 4.0;
    double octSpan = span / 8.0;

    for(int i = 0; i < getColorChannelHistograms().histogramY.size; i++)
    {
      double f = 1.0;
      if(histY.sum != 0)
      {
        double s = 0.0;
        if(i <= histY.common)
        {
          s = (histY.common - start) / 4.0;
        }
        else if(i > histY.common)
        {
          s = (end - histY.common) / 4.0;
        }
        f = gauss(s, histY.common, i);
      }
      else
      {
        f =  gauss(quadSpan, start + halfSpan, i);
      }
      DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
        PLOT_GENERIC("FieldColorClassifier:f_Y", i, f);
      );

      int val = (int) Math::round(getColorChannelHistograms().histogramY.rawData[i] * f);
      histY.add(i, val);
    }
    histY.calculate();

    double lowBorderY = histY.median - params.deviationFactor * histY.sigma;
    double highBorderY = histY.median + params.deviationFactor * histY.sigma;

    for(unsigned int i = 0; i < getColoredGrid().uniformGrid.size(); i++)
    {
      const Vector2i& point = getColoredGrid().uniformGrid.getPoint(i);

      getImage().get(point.x, point.y, pixel);

      if(lowBorderY < pixel.y && pixel.y < highBorderY)
      {
        histV.add(pixel.v);
      }
    }//end for
    start = getColorChannelHistograms().histogramV.min;
    end = getColorChannelHistograms().histogramV.max;
    span = getColorChannelHistograms().histogramV.spanWidth;
    halfSpan = span / 2.0;
    quadSpan = span / 4.0;
    octSpan = span / 8.0;
    if(histV.sum != 0)
      for(int i = 0; i < histV.size; i++)
      {
        double f = 1.0;
        f = gauss(quadSpan,  start + octSpan, i);

        DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
          PLOT_GENERIC("FieldColorClassifier:f_V", i, f);
        );
        histV.rawData[i] = (int) Math::round(histV.rawData[i] * f);
      }
    histV.calculate();


    double lowBorderV = histV.median - params.deviationFactor * histV.sigma;
    double highBorderV = histV.median + params.deviationFactor * histV.sigma;

    for(unsigned int i = 0; i < getColoredGrid().uniformGrid.size(); i++)
    {
      const Vector2i& point = getColoredGrid().uniformGrid.getPoint(i);
      getImage().get(point.x, point.y, pixel);

      if
      (
        lowBorderY < pixel.y && pixel.y < highBorderY
        &&
        lowBorderV < pixel.v && pixel.v < highBorderV
      )
      {
        histU.add(pixel.u);
      }
    }//end for
    start = getColorChannelHistograms().histogramU.min;
    span = getColorChannelHistograms().histogramU.spanWidth;
    halfSpan = span / 2.0;
    quadSpan = span / 4.0;
    octSpan = span / 8.0;
    for(int i = 0; i < histU.size; i++)
    {
      double f = gauss(quadSpan,  start + octSpan, i);
      DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
        PLOT_GENERIC("FieldColorClassifier:f_U", i, f);
      );
      histU.rawData[i] = (int) Math::round(histU.rawData[i] * f);
    }
    histU.calculate();
  }

  DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
    histV.plot("FieldColorClassifier:histV_1");
  );
  DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
    histU.plot("FieldColorClassifier:histU");
  );
  DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
    histY.plot("FieldColorClassifier:histY");
  );

  runDebugRequests();
}

double FieldColorClassifier::gauss(double sigma, double mean, double x)
{
  static const double f = 1.0 / sqrt(2.0 * Math::pi);
  double exponent = -((x - mean) * (x - mean)) / (2 * sigma * sigma);
  double ret = /* f / sigma **/ exp( exponent );
  return Math::isInf(ret) || Math::isNan(ret) ? 0.0 : ret;
}



void FieldColorClassifier::runDebugRequests()
{
  DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:set_in_image",
    double lowBorderV = histV.median - params.deviationFactor * histV.sigma;
    double highBorderV = histV.median + params.deviationFactor * histV.sigma;
    double lowBorderU = histU.median - params.deviationFactor * histU.sigma;
    double highBorderU = histU.median + params.deviationFactor * histU.sigma;
    double lowBorderY = histY.median - params.deviationFactor * histY.sigma;
    double highBorderY = histY.median + params.deviationFactor * histY.sigma;
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
      {
        const Pixel& pixel = getImage().get(x, y);
        //if(getFieldColorPercept().isFieldColor(pixel))
        if
        (
          lowBorderU < pixel.u && pixel.u < highBorderU
          &&
          lowBorderV < pixel.v && pixel.v < highBorderV
          &&
          lowBorderY < pixel.y && pixel.y < highBorderY
        )
        {
          POINT_PX(ColorClasses::green, x, y);
        }
      }
    }
  );
  DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:set_dev_V1",
    double lowBorderY = histY.median - params.deviationFactor * histY.sigma;
    double highBorderY = histY.median + params.deviationFactor * histY.sigma;
    double lowBorder = histV.median - params.deviationFactor * histV.sigma;
    double highBorder = histV.median + params.deviationFactor * histV.sigma;
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
      {
        const Pixel& pixel = getImage().get(x, y);
        if
        (
          lowBorderY < pixel.y && pixel.y < highBorderY
          &&
          lowBorder < pixel.v && pixel.v < highBorder
        )
        {
          POINT_PX(ColorClasses::red, x, y);
        }
      }
    }
  );
  DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:set_dev_Y1",
    double lowBorder = histY.median - params.deviationFactor * histY.sigma;
    double highBorder = histY.median + params.deviationFactor * histY.sigma;
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
      {
        const Pixel& pixel = getImage().get(x, y);
        if(lowBorder < pixel.y && pixel.y < highBorder)
        {
          POINT_PX(ColorClasses::black, x, y);
        }
      }
    }
  );
}


