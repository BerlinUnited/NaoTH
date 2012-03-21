/**
 * @file InertiaSensorCalibrator.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * 
 * Calibrating the sensors: Inertial, Accelerometer, Gyrometer.
 * Inspired by the BH-2011
 */

#ifndef _INERTIAL_FILTER_H_
#define _INERTIAL_FILTER_H_

#include "Motion/MotionBlackBoard.h"

#include "Representations/Infrastructure/CalibrationData.h"

#include "Tools/Math/Kalman.h"
#include "Tools/DataStructures/RingBuffer.h"
#include "Tools/DataStructures/RingBufferWithSum.h"

#include "Tools/Debug/NaoTHAssert.h"

class InertiaSensorCalibrator
{
public:
  InertiaSensorCalibrator(const MotionBlackBoard& bb, CalibrationData& theCalibrationData);
  void update();

private:
  bool intentionallyMoving();
  void reset();

private:
  const MotionBlackBoard& theBlackBoard;
  CalibrationData& theCalibrationData;

  // inertial
  Kalman<double> inertialXBias; /**< The calibration bias of inertialX. */
  Kalman<double> inertialYBias; /**< The calibration bias of inertialY. */

  // accelerometer
  Kalman<double> accXBias; /**< The calibration bias of accX. */
  Kalman<double> accYBias; /**< The calibration bias of accY. */
  Kalman<double> accZBias; /**< The calibration bias of accZ. */
  
  // gyro
  Kalman<double> gyroXBias; /**< The calibration bias of gyroX. */
  Kalman<double> gyroYBias; /**< The calibration bias of gyroY. */

  bool calibrated; /**< Whether the filters are initialized. */
  unsigned int lastTime; /**< The time of the previous iteration. */

  unsigned int collectionStartTime; /**< When the current collection was started. */
  unsigned int cleanCollectionStartTime; /**< When the last unstable situation was over. */


  RingBufferWithSum<Vector2d, 300> inertialValues; /**< Ringbuffer for collecting the acceleration sensor values of one walking phase or 1 sec. */
  RingBufferWithSum<Vector3d, 300> accValues; /**< Ringbuffer for collecting the acceleration sensor values of one walking phase or 1 sec. */
  RingBufferWithSum<Vector2d, 300> gyroValues; /**< Ringbuffer for collecting the gyro sensor values of one walking phase or 1 sec. */


  /**
  * Class for buffering averaged gyro and acceleration sensor readings.
  */
  class Collection
  {
  public:
    Vector2d inertialAvg; /**< The average of acceleration sensor readings of one walking phase or 1 sec. */
    Vector3d accAvg; /**< The average of acceleration sensor readings of one walking phase or 1 sec. */
    Vector2d gyroAvg; /**< The average of gyro sensor eadings of one walking phase or 1 sec. */
    unsigned int timeStamp; /**< When this collection was created. */

    /**
    * Constructs a collection.
    */
    Collection(
      const Vector2d& inertialAvg, 
      const Vector3d& accAvg, 
      const Vector2d& gyroAvg, 
      unsigned int timeStamp) 
      :
      inertialAvg(inertialAvg),
      accAvg(accAvg),
      gyroAvg(gyroAvg),
      timeStamp(timeStamp){}

    /**
    * Default constructor.
    */
    Collection() {}
  };


  
  RingBuffer<Collection, 50> collections; /**< Buffered averaged gyro and accleration sensor readings. */
};

#endif // _INERTIAL_FILTER_H_
