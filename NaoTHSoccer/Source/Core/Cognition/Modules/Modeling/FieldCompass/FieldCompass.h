/**
 * @file ActiveGoalLocator.h
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 * A fake field compass, just providing the rotation calculated by the 
 * self locator
 */

#ifndef _FieldCompass_h_
#define _FieldCompass_h_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/CompassDirection.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(FieldCompass)
  REQUIRE(RobotPose)

  PROVIDE(CompassDirection)
END_DECLARE_MODULE(FieldCompass)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class FieldCompass : private FieldCompassBase
{

public:
  FieldCompass();
  virtual ~FieldCompass(){};

  virtual void execute();

};

#endif //_FieldCompass_h_
