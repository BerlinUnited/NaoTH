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

    inline void increaseValue(const UniformGrid& grid, const int& pixelIndex, const ColorClasses::Color& color)
    {
      const Vector2<int>& pixel = grid.getGridCoordinates(pixelIndex);
      xHistogram[color][pixel.y]++;
      yHistogram[color][pixel.x]++;
    }//end increaseValue

    inline void increaseChannelValue(const Pixel& pixel, const ColorClasses::Color& color)
    {
      // calculate only once
      //static unsigned int factor = 256 / COLOR_CHANNEL_VALUE_COUNT;

      switch(color)
      {
        case ColorClasses::orange:
        case ColorClasses::red:
          colorChannelHistogramBall[0][pixel.y]++;
          colorChannelHistogramBall[1][pixel.u]++;
          colorChannelHistogramBall[2][pixel.v]++;
          break;
        case ColorClasses::yellow:
          colorChannelHistogramGoal[0][pixel.y]++;
          colorChannelHistogramGoal[1][pixel.u]++;
          colorChannelHistogramGoal[2][pixel.v]++;
          break;
        case ColorClasses::green:
          colorChannelHistogramField[0][pixel.y]++;
          colorChannelHistogramField[1][pixel.u]++;
          colorChannelHistogramField[2][pixel.v]++;
          break;
        case ColorClasses::white:
        case ColorClasses::gray:
          colorChannelHistogramLine[0][pixel.y]++;
          colorChannelHistogramLine[1][pixel.u]++;
          colorChannelHistogramLine[2][pixel.v]++;
          break;
        default: break;
      }//end switch

      colorChannelIsUptodate  = true;
    }//end increaseChannelValue


    void createFromColoredGrid(const ColoredGrid& coloredGrid);
    void showDebugInfos(const UniformGrid& grid, const CameraInfo& cameraInfo) const;
    virtual void print(ostream& stream) const;

  public:
    // FIXME: remove HACK_MAX_HEIGHT & HACK_MAX_WIDTH
    unsigned int xHistogram[ColorClasses::numOfColors][UniformGrid::HACK_MAX_HEIGHT];
    unsigned int yHistogram[ColorClasses::numOfColors][UniformGrid::HACK_MAX_WIDTH];

    unsigned int colorChannelHistogramField[3][COLOR_CHANNEL_VALUE_COUNT];
    unsigned int colorChannelHistogramLine[3][COLOR_CHANNEL_VALUE_COUNT];
    unsigned int colorChannelHistogramGoal[3][COLOR_CHANNEL_VALUE_COUNT];
    unsigned int colorChannelHistogramBall[3][COLOR_CHANNEL_VALUE_COUNT];
    unsigned int colorChannelIsUptodate;

};


#endif  /* _Histogram_H */

