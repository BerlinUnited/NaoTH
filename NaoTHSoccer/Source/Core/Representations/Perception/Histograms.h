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

    inline void increaseValue(const Vector2<int> pixel, const ColorClasses::Color& color)
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
#endif  /* _Histogram_H */

