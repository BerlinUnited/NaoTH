/**
* @file KalmanFilterBallLocator.h
*
* @author <a href="mailto:kaptur@informatik.hu-berlin.de">Christian Kaptur</a>
* Declaration of class KalmanFilterBallLocator
*/

#ifndef _KalmanFilterBallLocator_h_
#define _KalmanFilterBallLocator_h_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Perception/BallPercept.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/SituationStatus.h"

#include "Tools/Math/Matrix2x2.h"
#include "Tools/Math/Line.h"
#include "Tools/DataStructures/RingBuffer.h"
#include "KFBLParameters.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(KalmanFilterBallLocator)
  REQUIRE(BodyState)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(OdometryData)
  REQUIRE(MotionStatus)
  REQUIRE(KinematicChain)
  REQUIRE(SituationStatus)
  REQUIRE(BallPercept)
  REQUIRE(BallPerceptTop)

  PROVIDE(BallModel)
END_DECLARE_MODULE(KalmanFilterBallLocator)


//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class KalmanFilterBallLocator : private KalmanFilterBallLocatorBase
{

public:
  KalmanFilterBallLocator();
  virtual ~KalmanFilterBallLocator(){}

  virtual void execute();

private:
  KFBLParameters parameters;
  OdometryData lastRobotOdometry; //just for prebuffered percepts
  OdometryData lastRobotOdometryAll;

  struct LocalBallPercept
  {
    FrameInfo lastFrameInfoWhenBallSeen;
    Vector2d lastSeenBall;
  };

  //RingBuffer<BallPercept, 20> smoothingPerceptBuffer;
  RingBuffer<BallPercept, 20> badPerceptBuffer;

  //stores last 10 BallPercepts
  RingBuffer<LocalBallPercept, 10 > ballBuffer;

  Matrix2x2<double> I; // eye

  // Sx = (Kalman-Position x, Kalman-Speed x)
  // Sy = (Kalman-Position y, Kalman-Speed y)
  Vector2<double> Sx, Sy;

  Matrix2x2<double> Px, Qx, Kx, Rx; // Kalman Filter-Matrices for x-direction
  Matrix2x2<double> Py, Qy, Ky, Ry; // Kalman Filter-Matrices for y-direction

  Vector2<double> lastSeenBall;
  FrameInfo lastFrameInfoWhenBallSeen;
  FrameInfo lastFrameInfo;
  bool modelIsValid;
  bool wasReactiveInLastFrame;

  void executeKalman(BallPercept getPercept);

  void reset(BallPercept newPercept);
  void resetMatrices();
  void updateMotion(Vector2<double>& Sx_, Vector2<double>& Sy_);

  void predictByMotionModel(Vector2<double>& position, Vector2<double>& speed, double time);
  void predictByMotionModelContineously(Vector2<double>& position, Vector2<double>& speed, double time);

  void updatePreviewModel();

  void removePerceptsByFrameNumber(RingBuffer<BallPercept, 20>& buffer, const unsigned int maxFrames) const;


  const BallPercept& getBallPercept() const
  {
    if(!KalmanFilterBallLocatorBase::getBallPercept().ballWasSeen &&
        KalmanFilterBallLocatorBase::getBallPerceptTop().ballWasSeen) 
    {
      return KalmanFilterBallLocatorBase::getBallPerceptTop();
    } else {
      return KalmanFilterBallLocatorBase::getBallPercept();
    }
  }
};

#endif //_KalmanFilterBallLocator_h_


