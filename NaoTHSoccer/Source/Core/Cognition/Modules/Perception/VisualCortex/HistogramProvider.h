/**
* @file HistogramProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class HistogramProvider
*/

#ifndef _HistogramProvider_h_
#define _HistogramProvider_h_

#include <ModuleFramework/Module.h>

//Tools
#include "Tools/ImageProcessing/Histograms.h"

// Representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Modeling/ColorClassificationModel.h"

#include "Tools/DoubleCamHelpers.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(HistogramProvider)
  REQUIRE(Image)
  REQUIRE(ImageTop)

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
  }

private:
  UniformGrid uniformGrid; // subsampling of the image
  CameraInfo::CameraID cameraID;

  DOUBLE_CAM_REQUIRE(HistogramProvider, Image);
  DOUBLE_CAM_PROVIDE(HistogramProvider, ColorChannelHistograms);
};

#endif // _HistogramProvider_h_
