#ifndef PLAINKALMANFILTERBALLLOCATOR_H
#define PLAINKALMANFILTERBALLLOCATOR_H

#include <ModuleFramework/Module.h>

// representations
#include <Representations/Perception/BallPercept.h>
#include "Representations/Modeling/BallModel.h"

#include "Representations/Modeling/OdometryData.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Infrastructure/FrameInfo.h"

#include "KalmanFilter4d.h"

// debug
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugRequest.h"
#include "Representations/Infrastructure/FieldInfo.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(PlainKalmanFilterBallLocator)

// debug stuff
  PROVIDE(DebugDrawings)
  PROVIDE(DebugRequest)

  REQUIRE(FieldInfo)

//  PROVIDE(DebugModify)
//  PROVIDE(DebugPlot)
//  PROVIDE(DebugParameterList)

//  REQUIRE(BodyState)
  REQUIRE(FrameInfo)
  REQUIRE(OdometryData)
//  REQUIRE(MotionStatus)
  REQUIRE(KinematicChain)
//  REQUIRE(SituationStatus)

  REQUIRE(BallPercept)
  REQUIRE(BallPerceptTop)

  PROVIDE(BallModel)
END_DECLARE_MODULE(PlainKalmanFilterBallLocator)


//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class PlainKalmanFilterBallLocator : private PlainKalmanFilterBallLocatorBase
{


public:
    PlainKalmanFilterBallLocator();
    virtual ~PlainKalmanFilterBallLocator();

    virtual void execute();

// from other kalman filter ball locator
private:
    OdometryData lastRobotOdometry; //just for prebuffered percepts
    //OdometryData lastRobotOdometryAll;

    BallPercept newPercept;

    FrameInfo lastFrameInfo;


private:
    KalmanFilter4d filter;

    const double epsilon; // 10e-6

    const BallPercept& getBallPercept() const
    {
      if(!PlainKalmanFilterBallLocatorBase::getBallPercept().ballWasSeen &&
          PlainKalmanFilterBallLocatorBase::getBallPerceptTop().ballWasSeen)
      {
        return PlainKalmanFilterBallLocatorBase::getBallPerceptTop();
      } else {
        return PlainKalmanFilterBallLocatorBase::getBallPercept();
      }
    }

    void doDebugRequest();
};

#endif // PLAINKALMANFILTERBALLLOCATOR_H
