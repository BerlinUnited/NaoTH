/**
* @file HistogramProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class HistogramProvider
*/

#include "HistogramProvider.h"

// Debug
#include <Tools/Debug/Stopwatch.h>


HistogramProvider::HistogramProvider()
:
  uniformGrid(getImage().width(), getImage().height(), 60, 40),
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:Histograms:enable_debug", "Enables the debug output for the histogram", false);
}

void HistogramProvider::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(id);

  // fill the grid
  STOPWATCH_START("Histograms");
  calculateHistograms();
  STOPWATCH_STOP("Histograms");

  DEBUG_REQUEST("Vision:Histograms:enable_debug",
    getHistograms().showDebugInfos(uniformGrid, getImage().cameraInfo);
  );
}//end execute

// hier wird das Gitter eingefaerbt (die Faerbung erfolgt fuer beliebige Gitter gleich,
// daher wird es nicht im GridCreator gemacht)
void HistogramProvider::calculateHistograms()//const Grid& grid)//, ColoredGrid& coloredGrid, Histogram& histogram)
{
  getHistograms().init();
  getColorChannelHistograms().init();

  Pixel pixel;
  
  for(unsigned int i = 0; i <uniformGrid.numberOfGridPoints; i++)
  {
    const Vector2i& point = uniformGrid.getPoint(i);

    //getImage().getCorrected(point.x, point.y, pixel);
    getImage().get(point.x, point.y, pixel);

    // classify the color
    ColorClasses::Color currentPixelColor = getColorClassificationModel().getColorClass(pixel);

    const Vector2i& gridPoint = uniformGrid.getGridCoordinates(i);
    getHistograms().increaseValue(gridPoint, currentPixelColor);
    
    getColorChannelHistograms().increaseChannelValue(pixel);
  }//end for
  
  getColorChannelHistograms().histogramY.calculate();
  getColorChannelHistograms().histogramU.calculate();
  getColorChannelHistograms().histogramV.calculate();

}//end calculateColoredGrid

