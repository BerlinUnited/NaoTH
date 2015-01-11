/**
* @file HistogramOverTimeProvider.h
*
* @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
* Declaration of class HistogramOverTimeProvider
*/

#ifndef _HistogramOverTimeProvider_h_
#define _HistogramOverTimeProvider_h_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Perception/Histograms.h"
#include "Representations/Modeling/ColorClassificationModel.h"

#include "Tools/DoubleCamHelpers.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(HistogramOverTimeProvider)
  REQUIRE(FrameInfo)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(ColorChannelHistograms)
  REQUIRE(ColorChannelHistogramsTop)

  PROVIDE(OverTimeHistogram)
  PROVIDE(OverTimeHistogramTop)
END_DECLARE_MODULE(HistogramOverTimeProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class HistogramOverTimeProvider : public HistogramOverTimeProviderBase
{
public:

  HistogramOverTimeProvider();
  ~HistogramOverTimeProvider(){}


  /** executes the module */
  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);
  }

private:
  CameraInfo::CameraID cameraID;

  FrameInfo lastFrameInfo;

  class BufferedData
  {
  public:
    PixelT<double> lastMean;

    double lastMinY;
    double lastMaxY;
    double lastSpanWidthY;

    RingBufferWithSum<double, 100> meanY;
    RingBufferWithSum<double, 100> meanU;
    RingBufferWithSum<double, 100> meanV;

    RingBufferWithSum<double, 100> minY;
    RingBufferWithSum<double, 100> maxY;
    RingBufferWithSum<double, 100> spanWidthY;
  };

  BufferedData buffer;
  BufferedData bufferTop;

  BufferedData& getBuffer()
  {
    if(cameraID == CameraInfo::Top) 
    {
        return bufferTop;
    }
    return buffer;
  }

  DOUBLE_CAM_REQUIRE(HistogramOverTimeProvider, Image);
  DOUBLE_CAM_REQUIRE(HistogramOverTimeProvider, ColorChannelHistograms);

  DOUBLE_CAM_PROVIDE(HistogramOverTimeProvider, OverTimeHistogram);
};

#endif // _HistogramOverTimeProvider_h_
