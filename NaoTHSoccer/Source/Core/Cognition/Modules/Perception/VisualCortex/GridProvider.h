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
#include "Tools/ImageProcessing/Histogram.h"

// Representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Modeling/ColorClassificationModel.h"
#include "Representations/Perception/FieldColorPreProcessingPercept.h"
#include "Representations/Perception/CameraMatrix.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(GridProvider)
  REQUIRE(Image)
  REQUIRE(ColorClassificationModel)
  REQUIRE(FieldColorPreProcessingPercept)
  REQUIRE(FieldColorPercept)
  REQUIRE(CameraMatrix)

  PROVIDE(ColoredGrid)
  PROVIDE(Histogram)
END_DECLARE_MODULE(GridProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class GridProvider : public GridProviderBase
{
public:

  GridProvider();
  ~GridProvider(){}


  /** executes the module */
  void execute();

private:
  void calculateColoredGrid();
};

#endif // _GridProvider_h_
