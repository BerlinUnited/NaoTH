/*
 * File:   histograms.h
 * Author: claas
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

#include "Tools/DataStructures/Histogram.h"

//Representations
#include "Representations/Infrastructure/CameraInfo.h"
#include "ColoredGrid.h"


class Histograms: public naoth::Printable
{
 public:
    Histograms();
    ~Histograms(){}

    void init();

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
    void createFromColoredGridTop(const ColoredGrid& coloredGrid);
    void showDebugInfos(const UniformGrid& grid, const naoth::CameraInfo& cameraInfo) const;
    virtual void print(std::ostream& stream) const;

  public:
    //color class histograms bottom image
    Statistics::Histogram<naoth::IMAGE_HEIGHT> xHistogram[ColorClasses::numOfColors];
    Statistics::Histogram<naoth::IMAGE_WIDTH > yHistogram[ColorClasses::numOfColors];
};


class ColorChanelHistograms
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

  ColorChanelHistograms();
  void showDebugInfos() const;

public:
  // color channel histograms bottomImage
  Statistics::Histogram<VALUE_COUNT> histogramY;
  Statistics::Histogram<VALUE_COUNT> histogramU;
  Statistics::Histogram<VALUE_COUNT> histogramV;

  bool colorChannelIsUptodate;
};

class HistogramsTop : public Histograms{};
class ColorChanelHistogramsTop: public ColorChanelHistograms{};

#endif  /* _Histogram_H */

