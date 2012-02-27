/**
* @file ColorProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class ColorProvider
*/

#ifndef __ColorProvider_h_
#define __ColorProvider_h_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/ColorTable64.h"
#include "Representations/Modeling/ColorClassificationModel.h"
#include "Representations/Perception/BaseColorRegionPercept.h"
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/FieldColorPreProcessingPercept.h"
#include "Representations/Infrastructure/FrameInfo.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(ColorProvider)
  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPreProcessingPercept)
  REQUIRE(BaseColorRegionPercept)
  REQUIRE(FrameInfo)

  PROVIDE(ColorTable64)
  PROVIDE(ColorClassificationModel)
END_DECLARE_MODULE(ColorProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class ColorProvider : public ColorProviderBase
{
public:

  ColorProvider();
  virtual ~ColorProvider(){}

  /** executes the module */
  void execute();
};

#endif //__ColorProvider_h_
