/**
* @file HistogramOverTimeProvider.h
*
* @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
* Implementation of class HistogramOverTimeProvider
*/

#include "HistogramOverTimeProvider.h"

// Debug
#include <Tools/Debug/Stopwatch.h>


HistogramOverTimeProvider::HistogramOverTimeProvider()
:
  cameraID(CameraInfo::Bottom)
{
  lastFrameInfo = getFrameInfo();
  buffer.lastMean.y = getColorChannelHistograms().histogramY.mean;
  buffer.lastMean.u = getColorChannelHistograms().histogramU.mean;
  buffer.lastMean.v = getColorChannelHistograms().histogramV.mean;
  bufferTop.lastMean.y = getColorChannelHistogramsTop().histogramY.mean;
  bufferTop.lastMean.u = getColorChannelHistogramsTop().histogramU.mean;
  bufferTop.lastMean.v = getColorChannelHistogramsTop().histogramV.mean;

  buffer.lastMinY = 0;
  bufferTop.lastMinY = 0;
  buffer.lastMaxY = 255;
  bufferTop.lastMaxY = 255;  
  buffer.lastSpanWidthY = 0;
  bufferTop.lastSpanWidthY = 0;  
}

void HistogramOverTimeProvider::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(id);

  double timeDiff = getFrameInfo().getTimeInSeconds() - lastFrameInfo.getTimeInSeconds();
  double diff = fabs(getColorChannelHistograms().histogramY.mean - getBuffer().lastMean.y);
  if(diff > 5.0 || timeDiff > 5.0)
  {
    getBuffer().meanY.add(getColorChannelHistograms().histogramY.mean);
    getOverTimeHistogram().meanEnv.y = getBuffer().meanY.getAverage();
    getOverTimeHistogram().diff.y = diff;
    getBuffer().lastMean.y = getColorChannelHistograms().histogramY.mean;
  }
  getOverTimeHistogram().meanImg.y = getColorChannelHistograms().histogramY.mean;

  diff = fabs(getColorChannelHistograms().histogramU.mean - getBuffer().lastMean.u);
  if(diff > 5.0 || timeDiff > 5.0)
  {
    getBuffer().meanU.add(getColorChannelHistograms().histogramU.mean);
    getOverTimeHistogram().meanEnv.u = getBuffer().meanU.getAverage();
    getOverTimeHistogram().diff.u = diff;
    getBuffer().lastMean.u = getColorChannelHistograms().histogramU.mean;
  }
  getOverTimeHistogram().meanImg.u = getColorChannelHistograms().histogramU.mean;

  diff = fabs(getColorChannelHistograms().histogramV.mean - getBuffer().lastMean.v);
  if(diff > 5.0 || timeDiff > 5.0)
  {
    getBuffer().meanV.add(getColorChannelHistograms().histogramV.mean);
    getOverTimeHistogram().meanEnv.v = getBuffer().meanV.getAverage();
    getOverTimeHistogram().diff.v = diff;
    getBuffer().lastMean.v = getColorChannelHistograms().histogramV.mean;
  }
  getOverTimeHistogram().meanImg.v = getColorChannelHistograms().histogramV.mean;

  diff = fabs(getColorChannelHistograms().histogramY.min - getBuffer().lastMinY);
  if(diff > 5.0 || timeDiff > 5.0)
  {
    getBuffer().lastMinY = getColorChannelHistograms().histogramY.min;
    getBuffer().minY.add(getBuffer().lastMinY);
    getOverTimeHistogram().minEnv.y = getBuffer().minY.getAverage();
  }

  diff = fabs(getColorChannelHistograms().histogramY.max - getBuffer().lastMaxY);
  if(diff > 5.0 || timeDiff > 5.0)
  {
    getBuffer().lastMaxY = getColorChannelHistograms().histogramY.max;
    getBuffer().maxY.add(getBuffer().lastMaxY);
    getOverTimeHistogram().maxEnv.y = getBuffer().maxY.getAverage();
  }

  diff = fabs(getColorChannelHistograms().histogramY.spanWidth - getBuffer().lastSpanWidthY);
  if(diff > 5.0 || timeDiff > 5.0)
  {
    getBuffer().lastSpanWidthY = getColorChannelHistograms().histogramY.spanWidth;
    getBuffer().spanWidthY.add(getBuffer().lastSpanWidthY);
    getOverTimeHistogram().spanWidthEnv.y = (int) (getBuffer().spanWidthY.getAverage() + 0.5);
  }
}//end execute


 