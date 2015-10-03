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
#include "UpdateAssociationFunctions.h"
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

//  REQUIRE(BodyState)
  REQUIRE(FrameInfo)
  REQUIRE(OdometryData)

// for preview stuff
  REQUIRE(MotionStatus)
  REQUIRE(KinematicChain)

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
    std::vector<ExtendedKalmanFilter4d> filter;
    std::vector<ExtendedKalmanFilter4d>::const_iterator bestModel;

    const double epsilon; // 10e-6
    double area95Threshold;

    //double ballMass;
    double c_RR;

    unsigned int kickTime;

    void applyOdometryOnFilterState(ExtendedKalmanFilter4d& filter);

    void predict(ExtendedKalmanFilter4d& filter, double dt) const;

    /*    DEBUG STUFF    */
    void doDebugRequest();
    void doDebugRequestBeforPredictionAndUpdate();
    void doDebugRequestBeforUpdate();
    void drawFiltersOnField() const;
    void reloadParameters();

    class KFParameters:  public ParameterList
    {
     public:
        KFParameters() : ParameterList("KalmanFilter4dBallModel")
        {
            PARAMETER_REGISTER(processNoiseStdQ00) = 15;
            PARAMETER_REGISTER(processNoiseStdQ01) = 0;
            PARAMETER_REGISTER(processNoiseStdQ10) = 0;
            PARAMETER_REGISTER(processNoiseStdQ11) = 20;

            // experimental determined
            PARAMETER_REGISTER(measurementNoiseR00) =  0.00130217; //[rad^2]
            PARAMETER_REGISTER(measurementNoiseR10) = -0.00041764; //[rad^2]
            PARAMETER_REGISTER(measurementNoiseR11) =  0.00123935; //[rad^2]

            PARAMETER_REGISTER(initialStateStdP00) = 250;
            PARAMETER_REGISTER(initialStateStdP01) = 0;
            PARAMETER_REGISTER(initialStateStdP10) = 0;
            PARAMETER_REGISTER(initialStateStdP11) = 250;

            //PARAMETER_REGISTER(ballMass) = 0.026;
            PARAMETER_REGISTER(c_RR) = 0.0245;
            PARAMETER_REGISTER(area95Threshold) = 1000*1000;

            //thresholds for association functions
            PARAMETER_REGISTER(euclidThreshold) = Math::fromDegrees(10);
            PARAMETER_REGISTER(mahalanobisThreshold) = Math::fromDegrees(10);
            PARAMETER_REGISTER(maximumLikelihoodThreshold) = 0.0001;

            syncWithConfig();
        }

        double processNoiseStdQ00;
        double processNoiseStdQ01;
        double processNoiseStdQ10;
        double processNoiseStdQ11;

        double measurementNoiseR00;
        double measurementNoiseR10;
        double measurementNoiseR11;

        double initialStateStdP00;
        double initialStateStdP01;
        double initialStateStdP10;
        double initialStateStdP11;

        //double ballMass;
        double c_RR;
        double area95Threshold;

        double euclidThreshold;
        double mahalanobisThreshold;
        double maximumLikelihoodThreshold;
    } kfParameters;

    Measurement_Function_H h;
    UpdateAssociationFunction* updateAssociationFunction;

    // available update association value functions
    EuclideanUAF   euclid;
    MahalanobisUAF mahalanobis;
    LikelihoodUAF  likelihood;

    Eigen::Matrix2d processNoiseStdSingleDimension;
    Eigen::Matrix2d measurementNoiseCovariances;
    Eigen::Matrix2d initialStateStdSingleDimension;
};

#endif // PLAINKALMANFILTERBALLLOCATOR_H
