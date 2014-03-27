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

  histY.setMaxTotalSum(120 * getColoredGrid().uniformGrid.size());
  histY_1.setMaxTotalSum(120 * getColoredGrid().uniformGrid.size());
  histU.setMaxTotalSum(120 * getColoredGrid().uniformGrid.size());
  histV_1.setMaxTotalSum(120 * getColoredGrid().uniformGrid.size());
  histV_2.setMaxTotalSum(120 * getColoredGrid().uniformGrid.size());
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
        histV_1.add(pixel.v);
      }
    }//end for
    start = getColorChannelHistograms().histogramV.min;
    end = getColorChannelHistograms().histogramV.max;
    span = getColorChannelHistograms().histogramV.spanWidth;
    halfSpan = span / 2.0;
    quadSpan = span / 4.0;
    octSpan = span / 8.0;
    if(histV_1.sum != 0)
      for(int i = 0; i < histV_1.size; i++)
      {
        double f = 1.0;
        f = gauss(quadSpan,  start + octSpan, i);

        DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
          PLOT_GENERIC("FieldColorClassifier:f_V", i, f);
        );
        histV_1.rawData[i] = (int) Math::round(histV_1.rawData[i] * f);
      }
    histV_1.calculate();


    //double lowBorder = histV_1.median - params.fV1 * histV_1.sigma;
    //double highBorder = histV_1.median + params.fV1 * histV_1.sigma;
    double lowBorderV = histV_1.median - params.deviationFactor * histV_1.sigma;
    double highBorderV = histV_1.median + params.deviationFactor * histV_1.sigma;

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


    //double lowBorderV = histV_2.median - params.fV2 * histV_2.sigma;
    //double highBorderV = histV_2.median + params.fV2 * histV_2.sigma;
    //double lowBorderU = histU.median - params.fU * histU.sigma;
    //double highBorderU = histU.median + params.fU * histU.sigma;
    //for(unsigned int i = 0; i < getColoredGrid().uniformGrid.size(); i++)
    //{
    //  const Vector2i& point = getColoredGrid().uniformGrid.getPoint(i);
    //  getImage().get(point.x, point.y, pixel);

    //  if
    //  (
    //    lowBorderY <= pixel.y && pixel.y <= highBorderY
    //    &&
    //    lowBorderU <= pixel.u && pixel.u <= highBorderU
    //    &&
    //    lowBorderV <= pixel.v && pixel.v <= highBorderV
    //  )
    //  {
    //    histY.add(pixel.y);
    //  }
    //}//end for
    //for(int i = 0; i < histY.size; i++)
    //{
    //  double f = gauss(127.5, 127.5, i);

    //  histY.rawData[i] = (int) Math::round(histY.rawData[i] * f);
    //}
    //histY.calculate();
  }

  //DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
  //  histY_1.plot("FieldColorClassifier:histY_1");
  //);
  DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
    histV_1.plot("FieldColorClassifier:histV_1");
  );
  DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
    histV_2.plot("FieldColorClassifier:histV_2");
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
    double lowBorderV = histV_1.median - params.deviationFactor * histV_1.sigma;
    double highBorderV = histV_1.median + params.deviationFactor * histV_1.sigma;
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
    double lowBorder = histV_1.median - params.deviationFactor * histV_1.sigma;
    double highBorder = histV_1.median + params.deviationFactor * histV_1.sigma;
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
  //DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:set_dev_V2",
  //  double lowBorderY = histY.common - params.fY * histY.sigma;
  //  double highBorderY = histY.common + params.fY * histY.sigma;
  //  double lowBorder = histV_2.median - params.fV2 * histV_2.sigma;
  //  double highBorder = histV_2.median + params.fV2 * histV_2.sigma;
  //  for(unsigned int x = 0; x < getImage().width(); x++)
  //  {
  //    for(unsigned int y = 0; y < getImage().height(); y++)
  //    {
  //      const Pixel& pixel = getImage().get(x, y);
  //      if
  //      (
  //        lowBorderY <= pixel.y && pixel.y <= highBorderY
  //        &&
  //        lowBorder <= pixel.v && pixel.v <= highBorder
  //      )
  //      {
  //        POINT_PX(ColorClasses::orange, x, y);
  //      }
  //    }
  //  }
  //);
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
  //DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:set_dev_U",
  //  double lowBorderY = histY.min;// histY.common - params.fY * histY.sigma;
  //  double highBorderY =  histY.max;//histY.common + params.fY * histY.sigma;
  //  double lowBorderV = histV_1.median - params.fV1 * histV_1.sigma;
  //  double highBorderV = histV_1.median + params.fV1 * histV_1.sigma;
  //  double lowBorderU = histU.median - params.fU * histU.sigma;
  //  double highBorderU = histU.median + params.fU * histU.sigma;
  //  for(unsigned int x = 0; x < getImage().width(); x++)
  //  {
  //    for(unsigned int y = 0; y < getImage().height(); y++)
  //    {
  //      const Pixel& pixel = getImage().get(x, y);
  //      if
  //      (
  //        lowBorderY <= pixel.y && pixel.y <= highBorderY
  //        &&
  //        lowBorderU <= pixel.u && pixel.u <= highBorderU
  //        &&
  //        lowBorderV <= pixel.v && pixel.v <= highBorderV
  //      )
  //      {
  //        POINT_PX(ColorClasses::pink, x, y);
  //      }
  //    }
  //  }
  //);
}


