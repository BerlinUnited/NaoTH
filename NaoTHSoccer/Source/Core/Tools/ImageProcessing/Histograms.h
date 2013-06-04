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

    inline void increaseValueTop(const int& x, const int& y, const ColorClasses::Color& color)
    {
      xHistogramTop[color].add(y);
      yHistogramTop[color].add(x);
    }//end increaseValue

    inline void increaseValue(const Vector2<int> pixel, const ColorClasses::Color& color)
    {
      xHistogram[color].add(pixel.y);
      yHistogram[color].add(pixel.x);
    }//end increaseValue

    inline void increaseValueTop(const Vector2<int> pixel, const ColorClasses::Color& color)
    {
      xHistogramTop[color].add(pixel.y);
      yHistogramTop[color].add(pixel.x);
    }//end increaseValue

    inline void increaseChannelValue(const Pixel& pixel, const ColorClasses::Color& color)
    {
      histogramY.add(pixel.y);
      histogramU.add(pixel.u);
      histogramV.add(pixel.v);
      colorChannelIsUptodate  = true;
    }//end increaseChannelValue

    inline void increaseChannelValueTop(const Pixel& pixel, const ColorClasses::Color& color)
    {
      histogramTopY.add(pixel.y);
      histogramTopU.add(pixel.u);
      histogramTopV.add(pixel.v);
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
    
    //color class histograms top image
    // FIXME: remove HACK_MAX_HEIGHT & HACK_MAX_WIDTH
    Statistics::Histogram<UniformGrid::HACK_MAX_HEIGHT> xHistogramTop[ColorClasses::numOfColors];
    Statistics::Histogram<UniformGrid::HACK_MAX_WIDTH > yHistogramTop[ColorClasses::numOfColors];

    // color channel histograms bottomImage
    Statistics::Histogram<COLOR_CHANNEL_VALUE_COUNT> histogramY;
    Statistics::Histogram<COLOR_CHANNEL_VALUE_COUNT> histogramU;
    Statistics::Histogram<COLOR_CHANNEL_VALUE_COUNT> histogramV;

    // color channel histograms top image
    Statistics::Histogram<COLOR_CHANNEL_VALUE_COUNT> histogramTopY;
    Statistics::Histogram<COLOR_CHANNEL_VALUE_COUNT> histogramTopU;
    Statistics::Histogram<COLOR_CHANNEL_VALUE_COUNT> histogramTopV;

    unsigned int colorChannelIsUptodate;

};


#endif  /* _Histogram_H */

