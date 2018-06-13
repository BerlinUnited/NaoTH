#ifndef MULTIKALMANBALLLOCATOR_H
#define MULTIKALMANBALLLOCATOR_H

#include <ModuleFramework/Module.h>

// representations
#include <Representations/Perception/MultiBallPercept.h>
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/PlayerInfo.h"


#include "Representations/Modeling/OdometryData.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Motion/MotionStatus.h"

#include "Representations/Perception/CameraMatrix.h"

#include "BallHypothesis.h"
#include "UpdateAssociationFunctions.h"

// debug
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"

#include "Representations/Infrastructure/FieldInfo.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(MultiKalmanBallLocator)

// debug stuff
  PROVIDE(DebugDrawings)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugPlot)

  REQUIRE(FieldInfo)

  REQUIRE(BodyState)
  REQUIRE(PlayerInfo)
  REQUIRE(FrameInfo)
  REQUIRE(OdometryData)

// for preview stuff
  REQUIRE(MotionStatus)
  REQUIRE(KinematicChain)

  REQUIRE(MultiBallPercept)

// for extended kalman filter
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

  PROVIDE(BallModel)
END_DECLARE_MODULE(MultiKalmanBallLocator)


//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class MultiKalmanBallLocator : private MultiKalmanBallLocatorBase
{
public:
    MultiKalmanBallLocator();
    virtual ~MultiKalmanBallLocator();

    virtual void execute();

// from other kalman filter ball locator
private:
    OdometryData lastRobotOdometry;

    FrameInfo lastFrameInfo;

private:
    typedef std::vector<BallHypothesis> Filters;
    Filters filter;
    Filters::const_iterator bestModel;

    const double epsilon; // 10e-6
    double area95Threshold;

    //double ballMass;
    double c_RR;

private:
    void updateByPerceptsCool();
    
    void updateByPerceptsNormal();
    void updateByPerceptsNaive(CameraInfo::CameraID camera);

    void applyOdometryOnFilterState(ExtendedKalmanFilter4d& filter);

    void predict(ExtendedKalmanFilter4d& filter, double dt) const;

    Filters::const_iterator selectBestModel() const;

    void provideBallModel(const BallHypothesis &model);

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
            PARAMETER_REGISTER(area95Threshold) = 1000*1000*Math::pi;

            //thresholds for association functions
            PARAMETER_REGISTER(euclidThreshold) = Math::fromDegrees(10);
            PARAMETER_REGISTER(mahalanobisThreshold) = Math::fromDegrees(10);
            PARAMETER_REGISTER(maximumLikelihoodThreshold) = 0.0005;

            //AssymetricalBoolFilte parameters
            PARAMETER_REGISTER(g0) = 0.01;
            PARAMETER_REGISTER(g1) = 0.1;

            syncWithConfig();
        }

        double g0;
        double g1;

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

#endif // MULTIKALMANBALLLOCATOR_H
