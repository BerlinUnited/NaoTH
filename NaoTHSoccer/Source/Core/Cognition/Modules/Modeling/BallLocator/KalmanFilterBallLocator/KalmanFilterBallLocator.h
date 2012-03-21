/**
* @file KalmanFilterBallLocator.h
*
* @author <a href="mailto:kaptur@informatik.hu-berlin.de">Christian Kaptur</a>
* Declaration of class KalmanFilterBallLocator
*/

#ifndef __KalmanFilterBallLocator_h_
#define __KalmanFilterBallLocator_h_

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

#include "Tools/Math/Matrix2x2.h"
#include "Tools/Math/Line.h"
#include "Tools/DataStructures/RingBuffer.h"
#include "KFBLParameters.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(KalmanFilterBallLocator)
  REQUIRE(BallPercept)
  REQUIRE(BodyState)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(OdometryData)
  REQUIRE(MotionStatus)
  REQUIRE(KinematicChain)
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
  OdometryData lastRobotOdometry;

  struct BallPercept
  {
    FrameInfo lastFrameInfoWhenBallSeen;
    Vector2<double> lastSeenBall;
  };

  //stores last 10 BallPercepts
  RingBuffer<BallPercept, 10 > ballBuffer;

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

  void reset();
  void resetMatrices();
  void updateMotion(Vector2<double>& Sx_, Vector2<double>& Sy_);

  void predictByMotionModel(Vector2<double>& position, Vector2<double>& speed, double time);
  void predictByMotionModelContineously(Vector2<double>& position, Vector2<double>& speed, double time);

  void updatePreviewModel();
};

#endif //__KalmanFilterBallLocator_h_


