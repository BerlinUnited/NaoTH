/*
 * File:   histogram.h
 * Author: claas
 *
 * Created on 9. Juli 2009, 13:40
 */

#ifndef _Histogram_H
#define  _Histogram_H

#include <sstream>


//Tools
#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"
#include "Tools/ImageProcessing/ColorModelConversions.h"
#include "Tools/ImageProcessing/ImagePrimitives.h"
#include "Tools/DataStructures/Printable.h"

//Representations
#include "Representations/Infrastructure/CameraInfo.h"
#include "ColoredGrid.h"


#define COLOR_CHANNEL_VALUE_COUNT 256

class Histogram: public naoth::Printable
{
 public:
    Histogram();
    ~Histogram(){}

    void init();
    void execute();

    inline void increaseValue(const int& x, const int& y, const ColorClasses::Color& color)
    {
      xHistogram[color][y]++;
      yHistogram[color][x]++;
    }//end increaseValue

    inline void increaseValue(const Vector2<int> pixel, const ColorClasses::Color& color)
    {
      xHistogram[color][pixel.y]++;
      yHistogram[color][pixel.x]++;
    }//end increaseValue

    inline void increaseChannelValue(const Pixel& pixel, const ColorClasses::Color& color)
    {
      colorChannelHistogramY[pixel.y]++;
      colorChannelHistogramCb[pixel.u]++;
      colorChannelHistogramCr[pixel.v]++;
      colorChannelIsUptodate  = true;
    }//end increaseChannelValue

    void createFromColoredGrid(const ColoredGrid& coloredGrid);
    void showDebugInfos(const UniformGrid& grid, const naoth::CameraInfo& cameraInfo) const;
    virtual void print(std::ostream& stream) const;

  public:
    // FIXME: remove HACK_MAX_HEIGHT & HACK_MAX_WIDTH
    int xHistogram[ColorClasses::numOfColors][UniformGrid::HACK_MAX_HEIGHT];
    int yHistogram[ColorClasses::numOfColors][UniformGrid::HACK_MAX_WIDTH];

    // color histograms
    int colorChannelHistogramY[COLOR_CHANNEL_VALUE_COUNT]; // y u-cb v-cr
    int colorChannelHistogramCb[COLOR_CHANNEL_VALUE_COUNT];
    int colorChannelHistogramCr[COLOR_CHANNEL_VALUE_COUNT];

    unsigned int colorChannelIsUptodate;

};


#endif  /* _Histogram_H */

