/**
 * @file ActiveGoalLocator.h
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 * Declaration of class ActiveGoalLocatorSimpleParticle
 */

#ifndef __FieldCompass_h_
#define __FieldCompass_h_

#include <ModuleFramework/Module.h>

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"

// Representations

// Tools

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(FieldCompass)
  REQUIRE(PlayerInfo)
  REQUIRE(BodyState)
  REQUIRE(CameraMatrix)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(OdometryData)

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
