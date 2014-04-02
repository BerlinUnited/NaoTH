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
  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:FieldColorClassifier:set_dev_V", " ", false);
  //DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:FieldColorClassifier:set_dev_V2", " ", false);
  //DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:FieldColorClassifier:set_dev_U", " ", false);
  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:FieldColorClassifier:set_dev_Y", " ", false);

  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:FieldColorClassifier:set_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:FieldColorClassifier:enable_plot", " ", false);
  
  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:FieldColorClassifier:clear", " ", false);

  histY.setMaxTotalSum(120 * getColoredGrid().uniformGrid.size());
  histU.setMaxTotalSum(120 * getColoredGrid().uniformGrid.size());
  histV.setMaxTotalSum(120 * getColoredGrid().uniformGrid.size());

  histCommonY.setMaxTotalSum(1200);
  histSigmaY.setMaxTotalSum(1200);

  histCommonU.setMaxTotalSum(1200);
  histSigmaU.setMaxTotalSum(1200);
  
  histCommonV.setMaxTotalSum(1200);
  histSigmaV.setMaxTotalSum(1200);
}

void FieldColorClassifier::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);

//#ifdef QVGA
  //if(cameraID == CameraInfo::Bottom)
//#endif
  {
    Pixel pixel;

    DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:clear", 
      histY.clear();
      histU.clear();
      histV.clear();

      histCommonY.clear();
      histSigmaY.clear();

      histCommonU.clear();
      histSigmaU.clear();
  
      histCommonV.clear();
      histSigmaV.clear();
    );


    const Statistics::HistogramX& histogramY = getColorChannelHistograms().histogramY;
    double start = histogramY.min;
    double end = histogramY.max;
    double span = histogramY.spanWidth;
    double halfSpan = span / 2.0;
    double quadSpan = span / 4.0;
    double octSpan = span / 8.0;

    double common = 0.0;

    if(histY.sum != 0)
    {
       common = histCommonY.common;
       start = histY.min;
       end = histY.max;
    }

    for(int i = 0; i < getColorChannelHistograms().histogramY.size; i++)
    {
      double f = 1.0;
      if(histY.sum != 0)
      {
        double s = 0.0;
        if(i <= common)
        {
          s = fabs(common - start) / 3.0;
        }
        else if(i > common)
        {
          s = fabs(end - common) / 3.0;
        }
        f = gauss(s, common, i);
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

    histCommonY.add(histY.common);
    histSigmaY.add((int) histY.sigma);
    for(int i = 0; i < histCommonY.size; i++)
    {
      double f = 1.0;
      if(histCommonY.sum != 0)
      {
        f = gauss(params.deviationFactorY * histY.sigma, histCommonY.common, i);
      }
      histCommonY.rawData[i] = (int) (f *  histCommonY.rawData[i]);
      DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
        PLOT_GENERIC("FieldColorClassifier:f_commonY", i, f);
      );

      f = 1.0;  
      if(histSigmaY.sum != 0)
      {
        f = gauss(params.deviationFactorY * histSigmaY.common, histSigmaY.common, i);
      }
      histSigmaY.rawData[i] = (int) (f *  histSigmaY.rawData[i]);
      DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
        PLOT_GENERIC("FieldColorClassifier:f_sigmaY", i, f);
      );
    }
    histCommonY.calculate();
    histSigmaY.calculate();

    double lowBorderY = histCommonY.common - params.PostDeviationFactor * histY.sigma;
    double highBorderY = histCommonY.common + params.PostDeviationFactor * histY.sigma;

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
    {
      start = histV.min;
      end = histV.max;
    }
    for(int i = 0; i < getColorChannelHistograms().histogramV.size; i++)
    {
      double f = 1.0;
      if(histV.sum != 0)
      {
        double s = 0.0;
        if(i <= histCommonV.common)
        {
          s = histCommonV.common / 3.0;
        }
        else
        {
          s = fabs(end - histCommonV.common) / 3.0;
        }
        f = gauss(params.deviationFactorV * s, histCommonV.common, i);
      }
      else
      {
        f =  gauss(quadSpan, start, i);
      }

      DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
        PLOT_GENERIC("FieldColorClassifier:f_V", i, f);
      );
      histV.rawData[i] = (int) Math::round(histV.rawData[i] * f);
    }
    histV.calculate();

    histCommonV.add(histV.common);
    histSigmaV.add((int) histV.sigma);
    for(int i = 0; i < histCommonV.size; i++)
    {
      double f = 1.0;
      if(histCommonV.sum != 0)
      {
        f = gauss(params.deviationFactorV * histV.sigma, histCommonV.common, i);
      }
      histCommonV.rawData[i] = (int) (f *  histCommonV.rawData[i]);
      DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
        PLOT_GENERIC("FieldColorClassifier:f_commonV", i, f);
      );

      f = 1.0;  
      if(histSigmaV.sum != 0)
      {
        f = gauss(params.deviationFactorV * histSigmaV.common, histSigmaV.common, i);
      }
      histSigmaV.rawData[i] = (int) (f *  histSigmaV.rawData[i]);
      DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
        PLOT_GENERIC("FieldColorClassifier:f_sigmaV", i, f);
      );
    }
    histCommonV.calculate();
    histSigmaV.calculate();

    double lowBorderV = histCommonV.common - params.PostDeviationFactor * histV.sigma;
    double highBorderV = histCommonV.common + params.PostDeviationFactor * histV.sigma;

    for(unsigned int i = 0; i < getColoredGrid().uniformGrid.size(); i++)
    {
      const Vector2i& point = getColoredGrid().uniformGrid.getPoint(i);
      getImage().get(point.x, point.y, pixel);

      if
      (
        lowBorderY < pixel.y && pixel.y < highBorderY
        //&&
        //lowBorderV < pixel.v && pixel.v < highBorderV
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

    if(histU.sum != 0)
    {
      start = histU.min;
      end = histU.max;
    }
    for(int i = 0; i < getColorChannelHistograms().histogramU.size; i++)
    {
      double f = 1.0;
      if(histU.sum != 0)
      {
        double s = 0.0;
        if(i <= histCommonU.common)
        {
          s = histCommonU.common / 3.0;
        }
        else
        {
          s = fabs(end - histCommonU.common) / 3.0;
        }
        f = gauss(params.deviationFactorU * s, histCommonU.common, i);
      }
      else
      {
        f =  gauss(quadSpan, start, i);
      }

      DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
        PLOT_GENERIC("FieldColorClassifier:f_U", i, f);
      );
      histU.rawData[i] = (int) Math::round(histU.rawData[i] * f);
    }
    histU.calculate();

    histCommonU.add(histU.common);
    histSigmaU.add((int) histU.sigma);
    for(int i = 0; i < histCommonU.size; i++)
    {
      double  f = 1.0;
      if(histCommonU.sum != 0)
      {
        f = gauss(params.deviationFactorU * histU.sigma, histCommonU.common, i);
      }
      histCommonU.rawData[i] = (int) (f *  histCommonU.rawData[i]);
      DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
        PLOT_GENERIC("FieldColorClassifier:f_commonU", i, f);
      );

      f = 1.0;
      if(histSigmaU.sum != 0)
      {
        f = gauss(params.deviationFactorU * histSigmaU.common, histSigmaU.common, i);
      }    
      histSigmaU.rawData[i] = (int) (f *  histSigmaU.rawData[i]);
      DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
        PLOT_GENERIC("FieldColorClassifier:f_sigmaU", i, f);
      );
    }
    histCommonU.calculate();
    histSigmaU.calculate();
  }

  runDebugRequests();
}

void FieldColorClassifier::setPercept()
{
  int lowBorderV = (int)(histCommonV.common - params.PostDeviationFactor * histSigmaV.common);
  int highBorderV = (int)(histCommonV.common + params.PostDeviationFactor * histSigmaV.common);
  int lowBorderU = (int)(histCommonU.common - params.PostDeviationFactor * histSigmaU.common);
  int highBorderU = (int)(histCommonU.common + params.PostDeviationFactor * histSigmaU.common);
  int lowBorderY = (int)(histCommonY.common - params.deviationFactorY * histSigmaY.common);
  int highBorderY = (int)(histCommonY.common + params.deviationFactorY * histSigmaY.common);

  getFieldColorPercept().range.set(lowBorderY, lowBorderU, lowBorderV, highBorderY, highBorderU, highBorderV);
  getFieldColorPercept().lastUpdated = getFrameInfo();
  getFieldColorPerceptTop().range.set(lowBorderY, lowBorderU, lowBorderV, highBorderY, highBorderU, highBorderV);
  getFieldColorPerceptTop().lastUpdated = getFrameInfo();
}

double FieldColorClassifier::gauss(double sigma, double mean, double x)
{
  //static const double f = 1.0 / sqrt(2.0 * Math::pi);
  double exponent = -((x - mean) * (x - mean)) / (2 * sigma * sigma);
  double ret = /* f / sigma **/ exp( exponent );
  return Math::isInf(ret) || Math::isNan(ret) ? 0.0 : ret;
}



void FieldColorClassifier::runDebugRequests()
{
  DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:enable_plot",
    histY.plotNormalized("FieldColorClassifier:histY");
    histU.plotNormalized("FieldColorClassifier:histU");
    histV.plotNormalized("FieldColorClassifier:histV");

    histCommonY.plotNormalized("FieldColorClassifier:histCommonY");
    histSigmaY.plotNormalized("FieldColorClassifier:histSigmaY");

    histCommonU.plotNormalized("FieldColorClassifier:histCommonU");
    histSigmaU.plotNormalized("FieldColorClassifier:histSigmaU");

    histCommonV.plotNormalized("FieldColorClassifier:histCommonV");
    histSigmaV.plotNormalized("FieldColorClassifier:histSigmaV");
  );
  
  DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:set_in_image",
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
      {
        const Pixel& pixel = getImage().get(x, y);
        if(getFieldColorPercept().isFieldColor(pixel))
        {
          POINT_PX(ColorClasses::green, x, y);
        }
      }
    }
  );

  DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:set_dev_V",
    double lowBorderY = histCommonY.common - params.deviationFactorY * histSigmaY.common;
    double highBorderY = histCommonY.common + params.deviationFactorY * histSigmaY.common;
    double lowBorder = histCommonV.common - params.PostDeviationFactor * histSigmaV.common;
    double highBorder = histCommonV.common + params.PostDeviationFactor * histSigmaV.common;
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

  DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:set_dev_Y",
    double lowBorder = histCommonY.common - params.deviationFactorY * histSigmaY.common;
    double highBorder = histCommonY.common + params.deviationFactorY * histSigmaY.common;
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


