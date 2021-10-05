/**
 * @file UltraSoundObstacleDetector.h
 *
 * @author <a href="mailto:kaptur@informatik.hu-berlin.de">Christian Kaptur</a>
 * Declaration of class ObstacleDetector
 */

#ifndef _UltraSoundObstacleDetector_h_
#define _UltraSoundObstacleDetector_h_

#include <ModuleFramework/Module.h>

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/DataStructures/RingBufferWithSum.h"
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

BEGIN_DECLARE_MODULE(UltraSoundObstacleDetector)
  PROVIDE(DebugModify)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugDrawings)

  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(OdometryData)
  REQUIRE(UltraSoundReceiveData)

  PROVIDE(ObstacleModel)
END_DECLARE_MODULE(UltraSoundObstacleDetector)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class UltraSoundObstacleDetector : private UltraSoundObstacleDetectorBase
{
public:
  UltraSoundObstacleDetector();
  ~UltraSoundObstacleDetector();

  virtual void execute();

private:  
  
  class Parameters: public ParameterList
  {
  public: 
    Parameters(): ParameterList("USParameters")
    {
      
      PARAMETER_REGISTER(minBlockedDistance) = 350;

      // load from the file after registering all parameters
      syncWithConfig();
    }

    int minBlockedDistance;
  } params;

private:

  static const double maxValidDistance;
  static const double invalidDistanceValue;

  bool wasFrontBlockedInLastFrame;
  FrameInfo timeWhenFrontBlockStarted;

  // used to recognize whether a new percept is avaliable
  UltraSoundReceiveData lastValidUltraSoundReceiveData;

  RingBufferWithSum<double,5> bufferLeft;
  RingBufferWithSum<double,5> bufferRight;

  void provideToLocalObstacleModel();
  bool isNewDataAvaliable() const;
  void fillBuffer();
  void drawObstacleModel();
};

#endif //_UltraSoundObstacleDetector_h_


