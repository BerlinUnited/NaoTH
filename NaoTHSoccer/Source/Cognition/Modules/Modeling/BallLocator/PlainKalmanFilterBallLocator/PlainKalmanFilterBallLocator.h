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

#include <eigen3/Eigen/Eigen>

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
//  REQUIRE(KinematicChain)
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
    OdometryData lastRobotOdometry;
    //OdometryData lastRobotOdometryAll;

    FrameInfo lastFrameInfo;

private: // friction stuff
    class Event{
        public:

            enum EventType{
                normalEvent,
                finalEvent
            } type;

            Event(double& u, double time, EventType type):
                type(type),
                u_entry(&u),
                time(time)
            {
            }

            double* u_entry;
            double  time;

        bool operator<(const Event& e2) const{
            return time < e2.time;
        }

        Event& operator=(const Event& e2){
            this->u_entry = e2.u_entry;
            this->time    = e2.time;
            this->type    = e2.type;
            return *this;
        }

    };

private: // multi stuff?

    double mahalanobisDistanceToState(const KalmanFilter4d& filter, const Eigen::Vector2d& z) const;

    // non normalized value of probability density function of measurement Z at the filters state
    double evaluatePredictionWithMeasurement(const KalmanFilter4d& filter, const Eigen::Vector2d& z) const;

private:
    KalmanFilter4d filter;

    const double epsilon; // 10e-6

    //double ballMass;
    double c_RR;

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

    void applyOdometryOnFilterState(KalmanFilter4d& filter);

    void doDebugRequest();

    void reloadKFParameters();

    class KFParameters:  public ParameterList
    {
     public:
        KFParameters() : ParameterList("KalmanFilter4dBallModel")
        {
            PARAMETER_REGISTER(processNoiseQ00) = 3;
            PARAMETER_REGISTER(processNoiseQ01) = 0;
            PARAMETER_REGISTER(processNoiseQ10) = 0;
            PARAMETER_REGISTER(processNoiseQ11) = 3;

            PARAMETER_REGISTER(measurementNoiseR00) = 10;
            PARAMETER_REGISTER(measurementNoiseR11) = 10;
            //PARAMETER_REGISTER(ballMass) = 0.026;
            PARAMETER_REGISTER(c_RR) = 0.0001;

            syncWithConfig();
        }

        double processNoiseQ00;
        double processNoiseQ01;
        double processNoiseQ10;
        double processNoiseQ11;

        double measurementNoiseR00;
        double measurementNoiseR11;

        //double ballMass;
        double c_RR;
    } kfParameters;
};

#endif // PLAINKALMANFILTERBALLLOCATOR_H
