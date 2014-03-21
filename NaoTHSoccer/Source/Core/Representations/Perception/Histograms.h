/*
 * File:   Histograms.h
 * @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
 *
 * Created on 9. Juli 2009, 13:40
 */

#ifndef _Histograms_H
#define  _Histograms_H

#include <sstream>


//Tools
#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"
#include "Tools/ImageProcessing/ColorModelConversions.h"
#include "Tools/ImageProcessing/ImagePrimitives.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/RingBufferWithSum.h"
#include "Tools/DataStructures/Histogram.h"

//Representations
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Infrastructure/ColoredGrid.h"
#include "Representations/Infrastructure/FrameInfo.h"


class ColorClassesHistograms: public naoth::Printable
{
 public:
    ColorClassesHistograms();
    ~ColorClassesHistograms(){}

    void init();

    void resizeHistograms(Vector2i& size)
    {
      histSizes = size;
      for(int color = 0; color < ColorClasses::numOfColors; color++)
      {
        //histogram along x-axis needs height as size
        xHistogram[color].resize(histSizes.y);
        //histogram along y-axis needs width as size
        yHistogram[color].resize(histSizes.x);
      }
    }

    inline void increaseValue(const int x, const int y, const ColorClasses::Color& color)
    {
      xHistogram[color].add(y);
      yHistogram[color].add(x);
    }

    inline void increaseValue(const Vector2i pixel, const ColorClasses::Color& color)
    {
      xHistogram[color].add(pixel.y);
      yHistogram[color].add(pixel.x);
    }

    void createFromColoredGrid(const ColoredGrid& coloredGrid);

    void showDebugInfos(const UniformGrid& grid, const naoth::CameraInfo& cameraInfo) const;
    virtual void print(std::ostream& stream) const;

  public:
    //color class histograms bottom image
    Statistics::HistogramX xHistogram[ColorClasses::numOfColors];
    Statistics::HistogramX yHistogram[ColorClasses::numOfColors];

  private:
    Vector2i histSizes;  
};

class ColorClassesHistogramsTop : public ColorClassesHistograms{};


class ColorChannelHistograms
{
public: 
  static const int VALUE_COUNT = 256;

  inline void increaseChannelValue(const Pixel& pixel)
  {
    histogramY.add(pixel.y);
    histogramU.add(pixel.u);
    histogramV.add(pixel.v);
    colorChannelIsUptodate  = true;
  }

  void init()
  {
    colorChannelIsUptodate = false;
    histogramY.clear();
    histogramU.clear();
    histogramV.clear();
  }

  void calculate()
  {
    histogramY.calculate();
    histogramU.calculate();
    histogramV.calculate();
  }

  ColorChannelHistograms();
  void showDebugInfos() const;

public:
  // color channel histograms bottomImage
  Statistics::Histogram<VALUE_COUNT> histogramY;
  Statistics::Histogram<VALUE_COUNT> histogramU;
  Statistics::Histogram<VALUE_COUNT> histogramV;

  bool colorChannelIsUptodate;
};

class ColorChannelHistogramsTop: public ColorChannelHistograms{};


class OverTimeHistogram : public naoth::Printable
{
public: 
  PixelT<double> meanEnv;
  PixelT<double> meanImg;

  PixelT<double> minEnv;
  PixelT<double> maxEnv;

  PixelT<int> spanWidthEnv;

  PixelT<double> diff;

  OverTimeHistogram()
  {
    minEnv.y = 0.0;
    minEnv.u = 0.0;
    minEnv.v = 0.0;
    maxEnv.y = 255.0;
    maxEnv.u = 255.0;
    maxEnv.v = 255.0;

    spanWidthEnv.y = 255;
    spanWidthEnv.u = 255;
    spanWidthEnv.v = 255;

    diff.y = 0.0;
    diff.u = 0.0;
    diff.v = 0.0;
  }

  virtual void print(std::ostream& stream) const;
};

class OverTimeHistogramTop : public OverTimeHistogram{};

class GoalPostHistograms : public ColorChannelHistograms, public naoth::Printable 
{
public:
  Vector2d colU;
  Vector2d colV;
  bool valid;

  GoalPostHistograms()
  :
    colU(255, 255),
    valid(false)
  { }

  void calculate()
  {
    valid = false;
    ColorChannelHistograms::calculate();
    setBorders(3.0);
  }

  void setBorders(double deviationFactor)
  {
    valid = histogramU.sum >= 2 * histogramU.size && histogramV.sum >= 2 * histogramV.size;
    if(valid)
    {
    //.x = low border, .y = high border
      colU.x = histogramU.median - deviationFactor * histogramU.sigma;
      colU.y = histogramU.median + deviationFactor * histogramU.sigma;
      colV.x = histogramV.median - deviationFactor * histogramV.sigma;
      colV.y = histogramV.median + deviationFactor * histogramV.sigma;
    }
  }

  bool isPostColor(Pixel pixel) const
  {
    //.x = low border, .y = high border
    return colU.x < pixel.u && pixel.u < colU.y && colV.x < pixel.v && pixel.v < colV.y;
  }

   virtual void print(std::ostream& stream) const
   {
     stream << "GoalPostHistogram" << std::endl
            << "=================" << std::endl << std::endl
            << "histogram U channel" << std::endl
            << "min        : " << histogramU.min << std::endl
            << "max        : " << histogramU.max << std::endl
            << "mean       : " << histogramU.mean << std::endl
            << "median     : " << histogramU.median << std::endl
            << "spanWidth  : " << histogramU.spanWidth << std::endl
            << "sigma      : " << histogramU.sigma << std::endl
            << "sum        : " << histogramU.sum << std::endl
            << "maxTotalSum: " << histogramU.maxTotalSum << std::endl
            << std::endl
            << "histogramV" << std::endl
            << "min        : " << histogramV.min << std::endl
            << "max        : " << histogramV.max << std::endl
            << "mean       : " << histogramV.mean << std::endl
            << "median     : " << histogramV.median << std::endl
            << "spanWidth  : " << histogramV.spanWidth << std::endl
            << "sigma      : " << histogramV.sigma << std::endl
            << "sum        : " << histogramV.sum << std::endl
            << "maxTotalSum: " << histogramV.maxTotalSum
            << std::endl;

   }

};

#endif  /* _Histogram_H */

