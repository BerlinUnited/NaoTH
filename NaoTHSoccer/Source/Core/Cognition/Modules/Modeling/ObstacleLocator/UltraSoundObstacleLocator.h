/**
 * @file UltraSoundObstacleLocator.h
 *
 * @author <a href="mailto:kaptur@informatik.hu-berlin.de">Christian Kaptur</a>
 * Declaration of class ObstacleLocator
 */

#ifndef _UltraSoundObstacleLocator_h_
#define _UltraSoundObstacleLocator_h_

#include <ModuleFramework/Module.h>

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/DataStructures/RingBuffer.h"
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/NaoTHAssert.h"

// tools
#include "Tools/Math/Geometry.h"

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/UltraSoundData.h"
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Modeling/ObstacleModel.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/BodyState.h"


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(UltraSoundObstacleLocator)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(OdometryData)
  REQUIRE(UltraSoundReceiveData)

  PROVIDE(ObstacleModel)
  PROVIDE(UltraSoundSendData)
END_DECLARE_MODULE(UltraSoundObstacleLocator)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class UltraSoundObstacleLocator : private UltraSoundObstacleLocatorBase
{
public:
  UltraSoundObstacleLocator();

  ~UltraSoundObstacleLocator()
  {
  }

  virtual void execute();

private:

  static const double maxValidDistance;
  static const double minBlockedDistance;
  static const double invalidDistanceValue;

  bool wasFrontBlockedInLastFrame;
  FrameInfo timeWhenFrontBlockStarted;

  RingBuffer<double,12> bufferLeft;
  RingBuffer<double,12> bufferRight;

  void provideToLocalObstacleModel();
  void fillBuffer();
  void drawObstacleModel();
};

#endif //_UltraSoundObstacleLocator_h_


