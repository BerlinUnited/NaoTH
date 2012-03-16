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

  // store around 3s
  RingBuffer<ObstacleModel::Obstacle, 75 > obstacleBuffer;
  unsigned int lastTimeObstacleWasSeen;

  OdometryData lastRobotOdometry;

  unsigned int ageThreshold; /* max age of the considered data */
  double usOpeningAngle; // opening angle of the ultrasound detector (in radians)

  // unsigned int callCounter;
  // unsigned int calcEveryCall;

  // Functionality
  double getMean();
  double getMinimum();

  void updateBuffer();
  void ageBuffer();
  void ageGrid();
  void provideToLocalObstacleModel();
  void updateByOdometry(const Pose2D& odometryDelta);  
  void drawObstacleModel();
  //virtual void calculatePoints();?
};

#endif //_UltraSoundObstacleLocator_h_


