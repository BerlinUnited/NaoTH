/**
* @file HistogramProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class HistogramProvider
*/

#ifndef _HistogramProvider_h_
#define _HistogramProvider_h_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Perception/Histograms.h"
#include "Representations/Perception/BodyContour.h"

#include "Tools/DoubleCamHelpers.h"
#include "Tools/DataStructures/UniformGrid.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugPlot.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(HistogramProvider)
  PROVIDE(DebugRequest)
  PROVIDE(DebugPlot)

  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(BodyContour)
  REQUIRE(BodyContourTop)

  PROVIDE(ColorChannelHistograms)
  PROVIDE(ColorChannelHistogramsTop)
END_DECLARE_MODULE(HistogramProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class HistogramProvider : public HistogramProviderBase
{
public:

  HistogramProvider();
  ~HistogramProvider(){}

  /** executes the module */
  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);

    //reset camera ID to Bottom for debug
    cameraID = CameraInfo::Bottom;
    showDebugInfos();
  }

  void showDebugInfos() const;

private:
  CameraInfo::CameraID cameraID;
  UniformGrid uniformGrid; // subsampling of the image

  DOUBLE_CAM_REQUIRE(HistogramProvider, Image);
  DOUBLE_CAM_REQUIRE(HistogramProvider, BodyContour);

  DOUBLE_CAM_PROVIDE(HistogramProvider, ColorChannelHistograms);

  //generate plot for raw, normalized and cumulated histograms
  void plot(std::string id, Statistics::HistogramX& histogram) const
  {
    for(int i = 0; i < histogram.size; i++)
    {
      PLOT_GENERIC(id + ":rawHistogram", i, histogram.rawData[i]);
      if(histogram.isCalculated())
      {
        PLOT_GENERIC(id + ":normalizedHistogram", i, histogram.normalizedData[i]);
        PLOT_GENERIC(id + ":cumulativeHistogram", i, histogram.cumulativeData[i]);
      }
    }
  };

  //generate plot only for raw histogram
  void plotRaw(std::string id, Statistics::HistogramX& histogram) const
  {
    if(histogram.isCalculated())
    {
      for(int i = 0; i < histogram.size; i++)
      {
        PLOT_GENERIC(id + ":rawHistogram", i, histogram.rawData[i]);
      }
    }
  };

  //generate plot only for normalized histogram
  void plotNormalized(std::string id, Statistics::HistogramX& histogram) const
  {
    if(histogram.isCalculated())
    {
      for(int i = 0; i < histogram.size; i++)
      {
        PLOT_GENERIC(id + ":normalizedHistogram", i, histogram.normalizedData[i]);
      }
    }
  };

  //generate plot only for cumulated histogram
  void plotCumulated(std::string id, Statistics::HistogramX& histogram) const
  {
    if(histogram.isCalculated())
    {
      for(int i = 0; i < histogram.size; i++)
      {
        PLOT_GENERIC(id + ":cumulativeHistogram", i, histogram.cumulativeData[i]);
      }
    }
  };

};

#endif // _HistogramProvider_h_
