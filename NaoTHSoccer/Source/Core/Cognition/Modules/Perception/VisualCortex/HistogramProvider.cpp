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
{}

void HistogramProvider::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(id);

  // fill the grid
  STOPWATCH_START("Histograms");
  calculateHistograms();
  STOPWATCH_STOP("Histograms");

}//end execute

// hier wird das Gitter eingefaerbt (die Faerbung erfolgt fuer beliebige Gitter gleich,
// daher wird es nicht im GridCreator gemacht)
void HistogramProvider::calculateHistograms()//const Grid& grid)//, ColoredGrid& coloredGrid, Histogram& histogram)
{
  getColorChannelHistograms().init();

  Pixel pixel;
  
  for(unsigned int i = 0; i <uniformGrid.numberOfGridPoints; i++)
  {
    const Vector2i& point = uniformGrid.getPoint(i);

    //getImage().getCorrected(point.x, point.y, pixel);
    getImage().get(point.x, point.y, pixel);

    getColorChannelHistograms().increaseChannelValue(pixel);
  }//end for
  
  getColorChannelHistograms().histogramY.calculate();
  getColorChannelHistograms().histogramU.calculate();
  getColorChannelHistograms().histogramV.calculate();

}//end calculateColoredGrid

