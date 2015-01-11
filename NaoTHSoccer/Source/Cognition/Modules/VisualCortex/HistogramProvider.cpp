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
{
  DEBUG_REQUEST_REGISTER("Vision:ColorChannelHistograms:TopCam:plotY", "plot Y channel histogram bottom image", false);
  DEBUG_REQUEST_REGISTER("Vision:ColorChannelHistograms:TopCam:plotU", "plot U channel histogram bottom image", false);
  DEBUG_REQUEST_REGISTER("Vision:ColorChannelHistograms:TopCam:plotV", "plot V channel histogram bottom image", false);
  
  DEBUG_REQUEST_REGISTER("Vision:ColorChannelHistograms:BottomCam:plotY", "plot Y channel histogram bottom image", false);
  DEBUG_REQUEST_REGISTER("Vision:ColorChannelHistograms:BottomCam:plotU", "plot U channel histogram bottom image", false);
  DEBUG_REQUEST_REGISTER("Vision:ColorChannelHistograms:BottomCam:plotV", "plot V channel histogram bottom image", false);
}

void HistogramProvider::showDebugInfos() const
{
  DEBUG_REQUEST("Vision:ColorChannelHistograms:TopCam:plotY", 
    plot("Histograms:TopCam:Y", getColorChannelHistogramsTop().histogramY);
  );
  DEBUG_REQUEST("Vision:ColorChannelHistograms:TopCam:plotU", 
    plot("Histograms:TopCam:U", getColorChannelHistogramsTop().histogramU);
  );
  DEBUG_REQUEST("Vision:ColorChannelHistograms:TopCam:plotV", 
    plot("Histograms:TopCam:V", getColorChannelHistogramsTop().histogramV);
  );

  DEBUG_REQUEST("Vision:ColorChannelHistograms:BottomCam:plotY", 
    plot("Histograms:BottomCam:Y", getColorChannelHistograms().histogramY);
  );
  DEBUG_REQUEST("Vision:ColorChannelHistograms:BottomCam:plotU", 
    plot("Histograms:BottomCam:U", getColorChannelHistograms().histogramU);
  );
  DEBUG_REQUEST("Vision:ColorChannelHistograms:BottomCam:plotV", 
    plot("Histograms:BottomCam:V", getColorChannelHistograms().histogramV);
  );
}


void HistogramProvider::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  getColorChannelHistograms().init();

  Pixel pixel;
  
  for(unsigned int i = 0; i <uniformGrid.size(); i++)
  {
    const Vector2i& point = uniformGrid.getPoint(i);

    if(!getBodyContour().isOccupied(point))
    {
      getImage().get(point.x, point.y, pixel);

      getColorChannelHistograms().increaseChannelValue(pixel);
    }
  }//end for
  
  getColorChannelHistograms().histogramY.calculate();
  getColorChannelHistograms().histogramU.calculate();
  getColorChannelHistograms().histogramV.calculate();

}//end execute

