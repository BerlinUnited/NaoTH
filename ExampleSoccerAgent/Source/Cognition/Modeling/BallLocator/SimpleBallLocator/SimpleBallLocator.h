/**
* @file KalmanFilterBallLocator.h
*
* @author <a href="mailto:kaptur@informatik.hu-berlin.de">Christian Kaptur</a>
* Declaration of class KalmanFilterBallLocator
*/

#ifndef __KalmanFilterBallLocator_h_
#define __KalmanFilterBallLocator_h_

#include "Cognition/Cognition.h"

// Representations
#include "Representations/Perception/BallPercept.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Motion/MotionStatus.h"

#include "Tools/Math/Matrix2x2.h"
#include "Tools/Math/Line.h"
#include "Tools/DataStructures/RingBuffer.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(KalmanFilterBallLocator)
  REQUIRE(BallPercept)
  REQUIRE(FrameInfo)
  REQUIRE(OdometryData)
  REQUIRE(MotionStatus)
  PROVIDE(BallModel)
END_DECLARE_MODULE(KalmanFilterBallLocator)


//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class KalmanFilterBallLocator : private KalmanFilterBallLocatorBase
{

public:
  KalmanFilterBallLocator();
  ~KalmanFilterBallLocator(){};

  virtual void execute();

private:
  OdometryData lastRobotOdometry;

};

#endif //__KalmanFilterBallLocator_h_


