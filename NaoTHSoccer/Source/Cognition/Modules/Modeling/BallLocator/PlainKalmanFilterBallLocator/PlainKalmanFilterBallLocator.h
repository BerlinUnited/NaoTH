#ifndef PLAINKALMANFILTERBALLLOCATOR_H
#define PLAINKALMANFILTERBALLLOCATOR_H

#include <ModuleFramework/Module.h>

#include <eigen3/Eigen/Eigen>

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(PlainKalmanFilterBallLocator)
// debug

//  PROVIDE(DebugDrawings)
//  PROVIDE(DebugRequest)
//  PROVIDE(DebugModify)
//  PROVIDE(DebugPlot)
//  PROVIDE(DebugParameterList)

//  REQUIRE(BodyState)
//  REQUIRE(FrameInfo)
//  REQUIRE(FieldInfo)
//  REQUIRE(OdometryData)
//  REQUIRE(MotionStatus)
//  REQUIRE(KinematicChain)
//  REQUIRE(SituationStatus)

//  REQUIRE(BallPercept)
//  REQUIRE(BallPerceptTop)

//  PROVIDE(BallModel)
END_DECLARE_MODULE(PlainKalmanFilterBallLocator)


//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class PlainKalmanFilterBallLocator : private PlainKalmanFilterBallLocatorBase
{


public:
    PlainKalmanFilterBallLocator();
    virtual ~PlainKalmanFilterBallLocator();

    virtual void execute();


private:
    Eigen::Matrix4d F; // state transition matrix
    Eigen::Matrix4d B; // control matrix
    Eigen::Vector4d x; // state
    Eigen::Vector4d u; // control vector
    Eigen::Vector4d x_pre; // predicted state
    Eigen::Vector4d x_corr; // corrected state
    Eigen::Vector4d w; // noise

    Eigen::Matrix4d Q; // covariance matrix of process noise
    Eigen::Matrix4d P; // covariance matrix of current state
    Eigen::Matrix4d P_pre; // covariance matrix of predicted state
    Eigen::Matrix4d P_corr; // covariance matrix of corrected state

    Eigen::Vector4d z; // measurement
    Eigen::Matrix4d H; // state to measurement transformation matrix
    Eigen::Matrix4d R; // covariance matrix of measurement noise

    Eigen::Matrix4d K; // kalman gain

};

#endif // PLAINKALMANFILTERBALLLOCATOR_H
