/**
* @file GridProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class GridProvider
*/

#include "GridProvider.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
//#include "Tools/Debug/DebugImageDrawings.h"
#include <Tools/Debug/Stopwatch.h>


GridProvider::GridProvider()
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:show_grid", "show the image processing grid", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:show_classified_image", "draw the image represented by uniformGrid", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:Histogram:enable_debug", "Enables the debug output for the histogram", false);
}


void GridProvider::execute()
{  
  // fill the grid
  calculateColoredGrid();
  
  // make some debug
  DEBUG_REQUEST("ImageProcessor:Histogram:enable_debug",
    getHistogram().showDebugInfos(getColoredGrid().uniformGrid, getImage().cameraInfo);
  );

  DEBUG_REQUEST("ImageProcessor:show_classified_image",
    for(unsigned int x=0; x<getColoredGrid().uniformGrid.width; x++)
    {
      for(unsigned int y=0; y<getColoredGrid().uniformGrid.height; y++)
      {
        POINT_PX(getColoredGrid().pointsColors[getColoredGrid().getScaledImageIndex(x,y)], (unsigned int)x, (unsigned int)y);
      }
    }
  );//end DEBUG
}//end execute

// hier wird das Gitter eingefaerbt (die Faerbung erfolgt fuer beliebige Gitter glech,
// daher wird es nicht im GridCreator gemacht)
void GridProvider::calculateColoredGrid()//const Grid& grid)//, ColoredGrid& coloredGrid, Histogram& histogram)
{
  STOPWATCH_START("Histogram+ColoredGrid");
  getColoredGrid().reset();
  getHistogram().init();

  double grey = 0;

  for(unsigned int i = 0; i < getColoredGrid().uniformGrid.numberOfGridPoints; i++)
  {
    const Vector2<int>& point = getColoredGrid().uniformGrid.getPoint(i);

    Pixel pixel = getImage().get(point.x,point.y);
    grey += pixel.y;

    ColorClasses::Color currentPixelColor = getColorTable64().getColorClass(pixel);
    if(currentPixelColor == ColorClasses::none)
    {
      getColoredGrid().percentOfUnknownColors += getColoredGrid().singlePointRate;
    }
    if
    (
      currentPixelColor == ColorClasses::white ||
      currentPixelColor == ColorClasses::green ||
      currentPixelColor == ColorClasses::orange ||
      currentPixelColor == ColorClasses::skyblue ||
      currentPixelColor == ColorClasses::yellow
    )
    {
      getColoredGrid().percentOfKnownColors += getColoredGrid().singlePointRate;
    }
    getColoredGrid().setColor(i, currentPixelColor);
    getHistogram().increaseValue(getColoredGrid().uniformGrid, i, currentPixelColor);
  }//end for

  getColoredGrid().valid = false;
  if(getColoredGrid().percentOfUnknownColors < 85 && getColoredGrid().percentOfKnownColors > 20)
  {
    getColoredGrid().valid = true;
  }

  getColoredGrid().meanBrightness = grey / (getColoredGrid().uniformGrid.height * getColoredGrid().uniformGrid.width);

  STOPWATCH_STOP("Histogram+ColoredGrid");
  
  DEBUG_REQUEST("ImageProcessor:show_grid",
    for(unsigned int i = 0; i < getColoredGrid().uniformGrid.numberOfGridPoints; i++)
    {
      Vector2<int> point = getColoredGrid().uniformGrid.getPoint(i);
      POINT_PX(getColoredGrid().pointsColors[i], point.x, point.y);
    }//end for
  );
}//end calculateColoredGrid

