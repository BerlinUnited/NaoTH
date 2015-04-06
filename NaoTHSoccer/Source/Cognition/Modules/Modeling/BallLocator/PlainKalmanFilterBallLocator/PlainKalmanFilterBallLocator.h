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
//    class Event{
//        public:

//            enum EventType{
//                normalEvent,
//                finalEvent
//            } type;

//            Event(double& u, double time, EventType type):
//                type(type),
//                u_entry(&u),
//                time(time)
//            {
//            }

//            double* u_entry;
//            double  time;

//        bool operator<(const Event& e2) const{
//            return time < e2.time;
//        }

//        Event& operator=(const Event& e2){
//            this->u_entry = e2.u_entry;
//            this->time    = e2.time;
//            this->type    = e2.type;
//            return *this;
//        }

//    };

private: // multi stuff?

    double euclidianDistanceToState(const KalmanFilter4d& filter, const Eigen::Vector2d& z) const;

    // non normalized value of probability density function of measurement Z at the filters state
    double evaluatePredictionWithMeasurement(const KalmanFilter4d& filter, const Eigen::Vector2d& z) const;

private:
    std::vector<KalmanFilter4d> filter;
//    KalmanFilter4d filter;

    const double epsilon; // 10e-6
    double distanceThreshold;
    double stdThreshold;

    //double ballMass;
    double c_RR;

    const KalmanFilter4d* bestModel;

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

    void predict(KalmanFilter4d& filter, double dt);

    /*    DEBUG STUFF    */
    void doDebugRequest();
    void doDebugRequestBeforPredictionAndUpdate();
    void drawFiltersOnField();
    void reloadParameters();

    class KFParameters:  public ParameterList
    {
     public:
        KFParameters() : ParameterList("KalmanFilter4dBallModel")
        {
            PARAMETER_REGISTER(processNoiseStdQ00) = 3;
            PARAMETER_REGISTER(processNoiseStdQ01) = 0;
            PARAMETER_REGISTER(processNoiseStdQ10) = 0;
            PARAMETER_REGISTER(processNoiseStdQ11) = 3;

            PARAMETER_REGISTER(measurementNoiseStdR00) = 10;
            PARAMETER_REGISTER(measurementNoiseStdR11) = 10;

            PARAMETER_REGISTER(initialStateStdP00) = 250;
            PARAMETER_REGISTER(initialStateStdP01) = 0;
            PARAMETER_REGISTER(initialStateStdP10) = 0;
            PARAMETER_REGISTER(initialStateStdP11) = 250;

            //PARAMETER_REGISTER(ballMass) = 0.026;
            PARAMETER_REGISTER(c_RR) = 0.0001;
            PARAMETER_REGISTER(distanceThreshold) = 500;
            PARAMETER_REGISTER(stdThreshold) = 500;

            syncWithConfig();
        }

        double processNoiseStdQ00;
        double processNoiseStdQ01;
        double processNoiseStdQ10;
        double processNoiseStdQ11;

        double measurementNoiseStdR00;
        double measurementNoiseStdR11;

        double initialStateStdP00;
        double initialStateStdP01;
        double initialStateStdP10;
        double initialStateStdP11;

        //double ballMass;
        double c_RR;
        double distanceThreshold;
        double stdThreshold;
    } kfParameters;

    Eigen::Matrix2d processNoiseStdSingleDimension;
    Eigen::Matrix2d measurementNoiseStd;
    Eigen::Matrix2d initialStateStdSingleDimension;
};

#endif // PLAINKALMANFILTERBALLLOCATOR_H
