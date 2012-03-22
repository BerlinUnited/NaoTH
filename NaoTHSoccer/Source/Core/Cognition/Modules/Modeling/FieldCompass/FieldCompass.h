/**
 * @file ActiveGoalLocator.h
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 * Declaration of class ActiveGoalLocatorSimpleParticle
 */

#ifndef __FieldCompass_h_
#define __FieldCompass_h_

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
  ~FieldCompass(){};

  virtual void execute();

private:

};

#endif //__FieldCompass_h_
