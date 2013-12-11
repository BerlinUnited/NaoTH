/**
* @file GridProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class GridProvider
*/

#include "GridProvider.h"

// Debug
#include <Tools/Debug/Stopwatch.h>


GridProvider::GridProvider()
:
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:show_grid", "show the image processing grid", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:show_classified_image", "draw the image represented by uniformGrid", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:Histogram:enable_debug", "Enables the debug output for the histogram", false);
}

void GridProvider::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(id);

  // fill the grid
  STOPWATCH_START("Histogram+ColoredGrid");
  calculateColoredGrid();
  STOPWATCH_STOP("Histogram+ColoredGrid");

  // make some debug
  DEBUG_REQUEST("ImageProcessor:show_grid",
    for(unsigned int i = 0; i < getColoredGrid().uniformGrid.numberOfGridPoints; i++) {
      const Vector2i& point = getColoredGrid().uniformGrid.getPoint(i);
      POINT_PX(getColoredGrid().pointsColors[i], point.x, point.y);
    }
  );

  DEBUG_REQUEST("ImageProcessor:Histogram:enable_debug",
    getHistograms().showDebugInfos(getColoredGrid().uniformGrid, getImage().cameraInfo);
  );

  DEBUG_REQUEST("ImageProcessor:show_classified_image",
    for(int x=0; x<(int)getColoredGrid().uniformGrid.width; x++) {
      for(int y=0; y<(int)getColoredGrid().uniformGrid.height; y++) {
        POINT_PX(getColoredGrid().pointsColors[getColoredGrid().getScaledImageIndex(x,y)], x, y);
      }
    }
  );
}//end execute

// hier wird das Gitter eingefaerbt (die Faerbung erfolgt fuer beliebige Gitter gleich,
// daher wird es nicht im GridCreator gemacht)
void GridProvider::calculateColoredGrid()//const Grid& grid)//, ColoredGrid& coloredGrid, Histogram& histogram)
{
  getColoredGrid().reset();
  getHistograms().init();
  getColorChanelHistograms().init();

  unsigned int grey = 0;
  unsigned int red = 0;
  unsigned int blue = 0;
  Pixel pixel;
  
  for(unsigned int i = 0; i < getColoredGrid().uniformGrid.numberOfGridPoints; i++)
  {
    const Vector2i& point = getColoredGrid().uniformGrid.getPoint(i);

    //getImage().getCorrected(point.x, point.y, pixel);
    getImage().get(point.x, point.y, pixel);

    // TODO: check if it is needed
    // mean color
    grey += pixel.y; // used by BaseColorClassifier
    red  += pixel.u; // used by BaseColorClassifier
    blue += pixel.v; // used by BaseColorClassifier

    // classify the color
    ColorClasses::Color currentPixelColor = getColorClassificationModel().getColorClass(pixel);

    // remember the color in the grid
    getColoredGrid().setColor(i, currentPixelColor);

    const Vector2i& gridPoint = getColoredGrid().uniformGrid.getGridCoordinates(i);
    getHistograms().increaseValue(gridPoint, currentPixelColor);
    
    getColorChanelHistograms().increaseChannelValue(pixel);
  }//end for

  getColoredGrid().validate();
  
  unsigned int imgArea = getColoredGrid().uniformGrid.maxNumberOfPoints;
  getColoredGrid().meanBrightness = grey / imgArea;
  getColoredGrid().meanRed = red / imgArea;
  getColoredGrid().meanBlue = blue / imgArea;

  getColorChanelHistograms().histogramY.calculate();
  getColorChanelHistograms().histogramU.calculate();
  getColorChanelHistograms().histogramV.calculate();

}//end calculateColoredGrid

