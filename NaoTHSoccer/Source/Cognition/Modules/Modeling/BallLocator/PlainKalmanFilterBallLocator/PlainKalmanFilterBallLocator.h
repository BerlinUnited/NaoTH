#ifndef PLAINKALMANFILTERBALLLOCATOR_H
#define PLAINKALMANFILTERBALLLOCATOR_H

#include <ModuleFramework/Module.h>

// representations
#include <Representations/Perception/BallPercept.h>
#include "Representations/Modeling/BallModel.h"

#include "Representations/Modeling/OdometryData.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Motion/MotionStatus.h"

#include "Representations/Perception/CameraMatrix.h"

#include "ExtendedKalmanFilter4d.h"
#include "MeasurementFunctions.h"

// debug
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"

#include "Representations/Infrastructure/FieldInfo.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(PlainKalmanFilterBallLocator)

// debug stuff
  PROVIDE(DebugDrawings)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugPlot)

  REQUIRE(FieldInfo)

//  PROVIDE(DebugModify)
//  PROVIDE(DebugPlot)
//  PROVIDE(DebugParameterList)

//  REQUIRE(BodyState)
  REQUIRE(FrameInfo)
  REQUIRE(OdometryData)

// for preview stuff
  REQUIRE(MotionStatus)
  REQUIRE(KinematicChain)

// cooridinate change
//  REQUIRE(CameraMatrix)
//  REQUIRE(CameraMatrixTop)
//  REQUIRE(SituationStatus)

  REQUIRE(BallPercept)
  REQUIRE(BallPerceptTop)

// for extended kalman filter
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

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

    FrameInfo lastFrameInfo;

private:

    double distanceToState(const ExtendedKalmanFilter4d& filter, const Eigen::Vector2d& z) const;

private:
    std::vector<ExtendedKalmanFilter4d> filter;

    const double epsilon; // 10e-6
    double distanceThreshold;
    double stdThreshold;

    //double ballMass;
    double c_RR;

    const ExtendedKalmanFilter4d* bestModel;

    void applyOdometryOnFilterState(ExtendedKalmanFilter4d& filter);

    void predict(ExtendedKalmanFilter4d& filter, double dt);

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

            PARAMETER_REGISTER(measurementNoiseStdR00) = 0.01745329251; //[rad]
            PARAMETER_REGISTER(measurementNoiseStdR11) = 0.01745329251; //[rad]

            PARAMETER_REGISTER(initialStateStdP00) = 250;
            PARAMETER_REGISTER(initialStateStdP01) = 0;
            PARAMETER_REGISTER(initialStateStdP10) = 0;
            PARAMETER_REGISTER(initialStateStdP11) = 250;

            //PARAMETER_REGISTER(ballMass) = 0.026;
            PARAMETER_REGISTER(c_RR) = 0.0045;
            PARAMETER_REGISTER(distanceThreshold) = 0.17453292519;
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

    Measurement_Function_H h;

    Eigen::Matrix2d processNoiseStdSingleDimension;
    Eigen::Matrix2d measurementNoiseStd;
    Eigen::Matrix2d initialStateStdSingleDimension;
};

#endif // PLAINKALMANFILTERBALLLOCATOR_H
