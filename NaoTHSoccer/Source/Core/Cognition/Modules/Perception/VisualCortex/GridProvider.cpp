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
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:CamTop:show_grid", "show the image processing grid", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:CamBottom:show_grid", "show the image processing grid", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:CamTop:show_classified_image", "draw the image represented by uniformGrid", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:CamBottom:show_classified_image", "draw the image represented by uniformGrid", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:Histogram:CamTop:enable_debug", "Enables the debug output for the histogram", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:Histogram:CamBottom:enable_debug", "Enables the debug output for the histogram", false);
}


void GridProvider::execute()
{
  // fill the grid
  calculateColoredGrid();

  // make some debug
  DEBUG_REQUEST("ImageProcessor:Histogram:CamTop:enable_debug",
    getHistogramsTop().showDebugInfos(getColoredGridTop().uniformGrid, getImageTop().cameraInfo);
  );
  DEBUG_REQUEST("ImageProcessor:Histogram:CamBottom:enable_debug",
    getHistograms().showDebugInfos(getColoredGrid().uniformGrid, getImage().cameraInfo);
  );

  DEBUG_REQUEST("ImageProcessor:CamTop:show_classified_image",
    for(unsigned int x=0; x<getColoredGridTop().uniformGrid.width; x++)
    {
      for(unsigned int y=0; y<getColoredGridTop().uniformGrid.height; y++)
      {
        TOP_POINT_PX(getColoredGridTop().pointsColors[getColoredGridTop().getScaledImageIndex(x,y)], (unsigned int)x, (unsigned int)y);
      } 
    }
  );//end DEBUG
  DEBUG_REQUEST("ImageProcessor:CamBottom:show_classified_image",
    for(unsigned int x=0; x<getColoredGrid().uniformGrid.width; x++)
    {
      for(unsigned int y=0; y<getColoredGrid().uniformGrid.height; y++)
      {
        POINT_PX(getColoredGrid().pointsColors[getColoredGrid().getScaledImageIndex(x,y)], (unsigned int)x, (unsigned int)y);
      }
    }
  );//end DEBUG
}//end execute

// hier wird das Gitter eingefaerbt (die Faerbung erfolgt fuer beliebige Gitter gleich,
// daher wird es nicht im GridCreator gemacht)
void GridProvider::calculateColoredGrid()//const Grid& grid)//, ColoredGrid& coloredGrid, Histogram& histogram)
{
  STOPWATCH_START("Histogram+ColoredGrid");
  getColoredGrid().reset();
  getColoredGridTop().reset();
  getHistograms().init();
  getHistogramsTop().init();

  unsigned int grey = 0;
  unsigned int red = 0;
  unsigned int blue = 0;
  unsigned int greyTop = 0;
  unsigned int redTop = 0;
  unsigned int blueTop = 0;
  Pixel pixel;
  Pixel pixelTop;

  for(unsigned int i = 0; i < getColoredGrid().uniformGrid.numberOfGridPoints; i++)
  {
    const Vector2<int>& point = getColoredGrid().uniformGrid.getPoint(i);
    const Vector2<int>& pointTop = getColoredGridTop().uniformGrid.getPoint(i);

    //getImage().getCorrected(point.x, point.y, pixel);
    getImage().get(point.x, point.y, pixel);
    getImageTop().get(pointTop.x, pointTop.y, pixelTop);

    // TODO: check if it is needed
    // mean color
    grey += pixel.y; // used by BaseColorClassifier
    red  += pixel.u; // used by BaseColorClassifier
    blue += pixel.v; // used by BaseColorClassifier
    greyTop += pixelTop.y; // used by BaseColorClassifier
    redTop  += pixelTop.u; // used by BaseColorClassifier
    blueTop += pixelTop.v; // used by BaseColorClassifier

    // classify the color
    ColorClasses::Color currentPixelColor = getColorClassificationModel().getColorClass(pixel);
    ColorClasses::Color currentPixelColorTop = getColorClassificationModelTop().getColorClass(pixelTop);

    // remember the color in the grid
    getColoredGrid().setColor(i, currentPixelColor);
    getColoredGridTop().setColor(i, currentPixelColorTop);

    const Vector2<int>& gridPoint = getColoredGrid().uniformGrid.getGridCoordinates(i);
    getHistograms().increaseValue(gridPoint, currentPixelColor);
    getHistograms().increaseChannelValue(pixel, currentPixelColor);

    const Vector2<int>& gridPointTop = getColoredGridTop().uniformGrid.getGridCoordinates(i);
    getHistogramsTop().increaseValue(gridPointTop, currentPixelColorTop);
    getHistogramsTop().increaseChannelValue(pixelTop, currentPixelColorTop);
  }//end for

  getColoredGrid().validate();
  getColoredGridTop().validate();
  
  unsigned int imgArea = getColoredGrid().uniformGrid.maxNumberOfPoints;
  getColoredGrid().meanBrightness = grey / imgArea;
  getColoredGrid().meanRed = red / imgArea;
  getColoredGrid().meanBlue = blue / imgArea;

  unsigned int imgAreaTop = getColoredGrid().uniformGrid.maxNumberOfPoints;
  getColoredGridTop().meanBrightness = greyTop / imgAreaTop;
  getColoredGridTop().meanRed = redTop / imgAreaTop;
  getColoredGridTop().meanBlue = blueTop / imgAreaTop;

  getHistograms().histogramY.calculate();
  getHistograms().histogramU.calculate();
  getHistograms().histogramV.calculate();

  getHistogramsTop().histogramY.calculate();
  getHistogramsTop().histogramU.calculate();
  getHistogramsTop().histogramV.calculate();

  STOPWATCH_STOP("Histogram+ColoredGrid");

  DEBUG_REQUEST("ImageProcessor:CamTop:show_grid",
    for(unsigned int i = 0; i < getColoredGridTop().uniformGrid.numberOfGridPoints; i++)
    {
      Vector2<int> point = getColoredGridTop().uniformGrid.getPoint(i);
      TOP_POINT_PX(getColoredGridTop().pointsColors[i], point.x, point.y);
    }//end for
  );
  DEBUG_REQUEST("ImageProcessor:CamBottom:show_grid",
    for(unsigned int i = 0; i < getColoredGrid().uniformGrid.numberOfGridPoints; i++)
    {
      Vector2<int> point = getColoredGrid().uniformGrid.getPoint(i);
      POINT_PX(getColoredGrid().pointsColors[i], point.x, point.y);
    }//end for
  );

}//end calculateColoredGrid

