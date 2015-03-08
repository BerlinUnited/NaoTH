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
#include "Tools/Debug/DebugParameterList.h"
#include "Representations/Infrastructure/FieldInfo.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(PlainKalmanFilterBallLocator)

// debug stuff
  PROVIDE(DebugDrawings)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)

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

    void reloadParameters();

    class KFParameters:  public ParameterList
    {
     public:
        KFParameters() : ParameterList("KalmanFilter4dBallModel")
        {
            PARAMETER_REGISTER(processNoiseQ00) = 3;
            PARAMETER_REGISTER(processNoiseQ01) = 0;
            PARAMETER_REGISTER(processNoiseQ10) = 0;
            PARAMETER_REGISTER(processNoiseQ11) = 0;

            PARAMETER_REGISTER(measurementNoiseR00) = 100;
            PARAMETER_REGISTER(measurementNoiseR11) = 100;

            syncWithConfig();
        }

        double processNoiseQ00;
        double processNoiseQ01;
        double processNoiseQ10;
        double processNoiseQ11;

        double measurementNoiseR00;
        double measurementNoiseR11;
    } kfParameters;
};

#endif // PLAINKALMANFILTERBALLLOCATOR_H
