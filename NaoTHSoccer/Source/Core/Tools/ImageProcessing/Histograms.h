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


#define COLOR_CHANNEL_VALUE_COUNT 256

class Histograms: public naoth::Printable
{
 public:
    Histograms();
    ~Histograms(){}

    void init();
    void execute();

    inline void increaseValue(const int& x, const int& y, const ColorClasses::Color& color)
    {
      xHistogram[color].add(y);
      yHistogram[color].add(x);
    }//end increaseValue

    inline void increaseValue(const Vector2<int> pixel, const ColorClasses::Color& color)
    {
      xHistogram[color].add(pixel.y);
      yHistogram[color].add(pixel.x);
    }//end increaseValue

    inline void increaseChannelValue(const Pixel& pixel, const ColorClasses::Color& color)
    {
      histogramY.add(pixel.y);
      histogramU.add(pixel.u);
      histogramV.add(pixel.v);
      colorChannelIsUptodate  = true;
    }//end increaseChannelValue

    void createFromColoredGrid(const ColoredGrid& coloredGrid);
    void createFromColoredGridTop(const ColoredGrid& coloredGrid);
    void showDebugInfos(const UniformGrid& grid, const naoth::CameraInfo& cameraInfo) const;
    virtual void print(std::ostream& stream) const;

  public:
    //color class histograms bottom image
    // FIXME: remove HACK_MAX_HEIGHT & HACK_MAX_WIDTH
    Statistics::Histogram<UniformGrid::HACK_MAX_HEIGHT> xHistogram[ColorClasses::numOfColors];
    Statistics::Histogram<UniformGrid::HACK_MAX_WIDTH > yHistogram[ColorClasses::numOfColors];
    
    // color channel histograms bottomImage
    Statistics::Histogram<COLOR_CHANNEL_VALUE_COUNT> histogramY;
    Statistics::Histogram<COLOR_CHANNEL_VALUE_COUNT> histogramU;
    Statistics::Histogram<COLOR_CHANNEL_VALUE_COUNT> histogramV;

    unsigned int colorChannelIsUptodate;

};

class HistogramsTop : public Histograms
{
public:
  virtual ~HistogramsTop() {}
};


#endif  /* _Histogram_H */

