/**
* @file InertiaSensorFilter.h
* Declaration of module InertiaSensorFilter.
* @author Colin Graf (BH-2011)
* @author Heinrich Mellmann (NaoTH-2012)
* original from BH-2011
*/

#ifndef _InertiaSensorFilter_H_
#define _InertiaSensorFilter_H_


#include "Tools/Math/Matrix2x2.h"
#include "Tools/Math/Pose3D.h"
#include "Tools/Math/RotationMatrix.h"

#include <ModuleFramework/Module.h>

#include "Representations/Infrastructure/FrameInfo.h"
#include <Representations/Infrastructure/RobotInfo.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/GroundContactModel.h"
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include "Representations/Infrastructure/CalibrationData.h"
#include "Representations/Modeling/InertialModel.h"
#include "Representations/Modeling/KinematicChain.h"

#include "Matrix3x2.h"

#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugPlot.h"

BEGIN_DECLARE_MODULE(InertiaSensorFilter)

  PROVIDE(DebugModify)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)

  REQUIRE(FrameInfo)
  REQUIRE(RobotInfo)
  REQUIRE(InertialSensorData)
  REQUIRE(GyrometerData)
  REQUIRE(AccelerometerData)
  REQUIRE(CalibrationData)
  REQUIRE(GroundContactModel)
  REQUIRE(MotionStatus)
  REQUIRE(KinematicChainSensor)

  PROVIDE(InertialModel)
END_DECLARE_MODULE(InertiaSensorFilter)


/**
* @class InertiaSensorFilter
* A module for estimating velocity and orientation of the torso.
*/
class InertiaSensorFilter: private InertiaSensorFilterBase
{
public:
  /** Default constructor. */
  InertiaSensorFilter();

  /**
  * Updates the OrientationData representation.
  * @param orientationData The orientation data representation which is updated by this module.
  */
  void execute();

private:
  /**
  * A collection of parameters for this module.
  */
  class Parameters
  {
  public:
    Vector2d processNoise; /**< The standard deviation of the process. */
    Vector3d accNoise; /**< The standard deviation of the inertia sensor. */
    Vector2d calculatedAccLimit; /**< Use a calculated angle up to this angle (in rad). (We use the acceleration sensors otherwise.) */

    Matrix2x2<double> processCov; /**< The covariance matrix for process noise. */
    Matrix3x3<double> sensorCov; /**< The covariance matrix for sensor noise. */

    Parameters() {}

    /**
    * Calculates parameter dependent constants used to speed up some calculations.
    */
    void calculateConstants();
  };


  /**
  * Represents the state to be estimated.
  */
  template <class V = double> class State
  {
  public:
    RotationMatrix rotation; /** The rotation of the torso. */

    /** Default constructor. */
    State() {}

    /**
    * Adds some world rotation given as angle axis.
    * @param value The flat vector to add.
    * @return A new object after the calculation.
    */
    State operator+(const Vector2<V>& value) const {
      return State(*this) += value;
    }

    /**
    * Adds some world rotation given as angle axis.
    * @param value The flat vector to add.
    * @return A reference this object after the calculation.
    */
    State& operator+=(const Vector2<V>& value) {
      rotation *= RotationMatrix(rotation.invert() * Vector3<V>(value.x, value.y, V()));
      return *this;
    }

    /**
    * Calculates a flat difference vector of two states.
    * @return The difference.
    */
    Vector2<V> operator-(const State& other) const {
      const Vector3<V>& rotDiff(other.rotation * ((const RotationMatrix&)(other.rotation.invert() * rotation)).getAngleAxis());
      return Vector2<V>(rotDiff.x, rotDiff.y);
    }
  };

  Parameters p; /**< The parameters of this module. */

  unsigned int lastTime; /**< The frame time of the previous iteration. */
  Vector2d safeRawAngle; /**< The last not corrupted angle from aldebarans angle estimation algorithm. */

  State<> x; /**< The estimate */
  Matrix2d cov; /**< The covariance of the estimate. */
  Matrix2d l; /**< The last caculated cholesky decomposition of \c cov. */
  State<> sigmaPoints[5]; /**< The last calculated sigma points. */

  Vector3d sigmaReadings[5]; /**< The expected sensor values at the sigma points. */
  Vector3d readingMean; /**< The mean of the expected sensor values which was determined by using the sigma velocities. */
  Matrix3d readingsCov;
  Matrix3x2d readingsSigmaPointsCov;

  /**
  * Restores the initial state.
  */
  void reset();

  void predict(const RotationMatrix& rotationOffset);
  void readingUpdate(const Vector3d& reading);

  void cholOfCov();
  void generateSigmaPoints();
  void meanOfSigmaPoints();
  void covOfSigmaPoints();

  void readingModel(const State<>& sigmaPoint, Vector3d& reading);
  void meanOfSigmaReadings();
  void covOfSigmaReadingsAndSigmaPoints();
  void covOfSigmaReadings();


  inline Matrix2d tensor(const Vector2d& a, const Vector2d& b) const {
    return Matrix2d(a * b.x, a * b.y);
  }

  inline Matrix2d tensor(const Vector2d& a) const {
    return Matrix2d(a * a.x, a * a.y);
  }

  inline Matrix3x2d tensor(const Vector3d& a, const Vector2d& b) const {
    return Matrix3x2d(a * b.x, a * b.y);
  }

  inline Matrix3d tensor(const Vector3d& a) const {
    return Matrix3d(a * a.x, a * a.y, a * a.z);
  }
};

#endif //_InertiaSensorFilter_H_
