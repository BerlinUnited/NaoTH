/**
* @file HistogramProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class HistogramProvider
*/

#include "HistogramProvider.h"

HistogramProvider::HistogramProvider()
:
  cameraID(CameraInfo::Bottom),
  uniformGrid(getImage().width(), getImage().height(), 60, 40)
{}

void HistogramProvider::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  //CANVAS_PX(id);

  getColorChannelHistograms().init();

  Pixel pixel;
  
  for(unsigned int i = 0; i <uniformGrid.size(); i++)
  {
    const Vector2i& point = uniformGrid.getPoint(i);

    if(!getBodyContour().isOccupied(point))
    {
      //getImage().getCorrected(point.x, point.y, pixel);
      getImage().get(point.x, point.y, pixel);

      getColorChannelHistograms().increaseChannelValue(pixel);
    }
  }//end for
  
  getColorChannelHistograms().histogramY.calculate();
  getColorChannelHistograms().histogramU.calculate();
  getColorChannelHistograms().histogramV.calculate();

}//end execute

