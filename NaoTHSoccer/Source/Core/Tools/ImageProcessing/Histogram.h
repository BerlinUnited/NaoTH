/* 
 * File:   histogram.h
 * Author: claas
 *
 * Created on 9. Juli 2009, 13:40
 */

#ifndef _Histogram_H
#define	_Histogram_H

#include <sstream>


#include "ColoredGrid.h"

//Tools
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"

//Representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Infrastructure/ColorTable64.h"

#include "Tools/ImageProcessing/ColorModelConversions.h"
#include "Tools/DataStructures/Printable.h"

#define COLOR_CHANNEL_VALUE_COUNT 256

class Histogram: public naoth::Printable
{
 public:
    Histogram();
    ~Histogram()
    {};

    void init();
    void execute();

    void increaseValue(const int& x, const int& y, const ColorClasses::Color& color);
    void increaseValue(const UniformGrid& grid, const int& pixelIndex, const ColorClasses::Color& color);
    void createFromColoredGrid(const ColoredGrid& coloredGrid);

    void increaseChannelValue(const Pixel& pixel);


    void showDebugInfos(const UniformGrid& grid, const CameraInfo& cameraInfo) const;
    virtual void print(ostream& stream) const;

  public:
    // FIXME: remove HACK_MAX_HEIGHT & HACK_MAX_WIDTH
    unsigned int xHistogram[ColorClasses::numOfColors][UniformGrid::HACK_MAX_HEIGHT];
    unsigned int yHistogram[ColorClasses::numOfColors][UniformGrid::HACK_MAX_WIDTH];
    unsigned int colorChannelHistogram[3][COLOR_CHANNEL_VALUE_COUNT];
};


#endif	/* _Histogram_H */

