/**
* @file GridProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class GridProvider
*/

#include "GridProvider.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
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

  unsigned int grey = 0;
  unsigned int red = 0;
  unsigned int blue = 0;
  Pixel pixel;

  for(unsigned int i = 0; i < getColoredGrid().uniformGrid.numberOfGridPoints; i++)
  {
    const Vector2<int>& point = getColoredGrid().uniformGrid.getPoint(i);

    getImage().get(point.x, point.y, pixel);

    // TODO: check if it is needed
    // mean color
    grey += pixel.y; // used by BaseColorClassifier
    red  += pixel.u; // used by BaseColorClassifier
    blue += pixel.v; // used by BaseColorClassifier

    // classify the color
    ColorClasses::Color currentPixelColor = getColorClassificationModel().getColorClass(pixel.y, pixel.u, pixel.v);

    // remember the color in the grid
    getColoredGrid().setColor(i, currentPixelColor);

    const Vector2<int>& gridPoint = getColoredGrid().uniformGrid.getGridCoordinates(i);

    getHistogram().increaseValue(gridPoint, currentPixelColor);
    getHistogram().increaseChannelValue(pixel, currentPixelColor);
    if(getFieldColorPreProcessingPercept().isFieldCromaRed(pixel.v))
    {
      getHistogram().collectFieldValue(pixel);
    }
  }//end for

  // 
  getColoredGrid().validate();
  

  unsigned int imgArea = getColoredGrid().uniformGrid.maxNumberOfPoints;
  getColoredGrid().meanBrightness = grey / imgArea;
  getColoredGrid().meanRed = red / imgArea;
  getColoredGrid().meanBlue = blue / imgArea;

  STOPWATCH_STOP("Histogram+ColoredGrid");


  DEBUG_REQUEST("ImageProcessor:show_grid",
    for(unsigned int i = 0; i < getColoredGrid().uniformGrid.numberOfGridPoints; i++)
    {
      Vector2<int> point = getColoredGrid().uniformGrid.getPoint(i);
      POINT_PX(getColoredGrid().pointsColors[i], point.x, point.y);
    }//end for
  );
}//end calculateColoredGrid

