/**
* @file GridProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class GridProvider
*/

#ifndef _GridProvider_h_
#define _GridProvider_h_

#include <ModuleFramework/Module.h>

//Tools
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/Histograms.h"

// Representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Modeling/ColorClassificationModel.h"
#include "Representations/Perception/CameraMatrix.h"

#include "Tools/DoubleCamHelpers.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(GridProvider)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(ColorClassificationModel)
  REQUIRE(ColorClassificationModelTop)
  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

  PROVIDE(ColoredGrid)
  PROVIDE(ColoredGridTop)
  PROVIDE(Histograms)
  PROVIDE(HistogramsTop)

  PROVIDE(ColorChannelHistograms)
  PROVIDE(ColorChannelHistogramsTop)
END_DECLARE_MODULE(GridProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class GridProvider : public GridProviderBase
{
public:

  GridProvider();
  ~GridProvider(){}


  /** executes the module */
  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);
  }

private:
  CameraInfo::CameraID cameraID;

  void calculateColoredGrid();

  DOUBLE_CAM_REQUIRE(GridProvider, Image);
  DOUBLE_CAM_REQUIRE(GridProvider, ColorClassificationModel);
  DOUBLE_CAM_REQUIRE(GridProvider, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(GridProvider, CameraMatrix);

  DOUBLE_CAM_PROVIDE(GridProvider, ColoredGrid);
  DOUBLE_CAM_PROVIDE(GridProvider, Histograms);
  DOUBLE_CAM_PROVIDE(GridProvider, ColorChannelHistograms);
};

#endif // _GridProvider_h_
