/**
 * @file InertialFilter.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef _INERTIAL_FILTER_H_
#define _INERTIAL_FILTER_H_

#include "Motion/MotionBlackBoard.h"

#include "Tools/Math/Kalman.h"
#include "Tools/DataStructures/RingBuffer.h"
#include "Tools/DataStructures/RingBufferWithSum.h"
#include "Tools/Debug/NaoTHAssert.h"

class InertialFilter
{
public:
  InertialFilter(const MotionBlackBoard& bb, Vector2d& offset);

  InertialPercept filter();

private:
  void calibrate();

  bool intentionallyMoving();

  void reset_bhuman_like();
  void update_bhuman_like();

private:
  const MotionBlackBoard& theBlackBoard;
  const naoth::InertialSensorData& sensorData;

  Vector2d& theOffset;
  Vector2d theNewOffset;
  unsigned int calibrateNum;
  const double max_offet;


  Kalman<double> inertialXBias; /**< The calibration bias of inertialX. */
  Kalman<double> inertialYBias; /**< The calibration bias of inertialY. */


  bool calibrated; /**< Whether the filters are initialized. */
  unsigned int lastTime; /**< The time of the previous iteration. */
  RotationMatrix calculatedRotation; /**< Body rotation, which was calculated using kinematics. */

  unsigned int collectionStartTime; /**< When the current collection was started. */
  unsigned int cleanCollectionStartTime; /**< When the last unstable situation was over. */

  RingBufferWithSum<Vector2d, 300> inertialValues; /**< Ringbuffer for collecting the acceleration sensor values of one walking phase or 1 sec. */






  /**
  * Class for buffering averaged gyro and acceleration sensor readings.
  */
  class Collection
  {
  public:
    Vector2d inertialAvg; /**< The average of acceleration sensor readings of one walking phase or 1 sec. */
    unsigned int timeStamp; /**< When this collection was created. */

    /**
    * Constructs a collection.
    */
    Collection(const Vector2d& inertialAvg, unsigned int timeStamp) :
      inertialAvg(inertialAvg), timeStamp(timeStamp){}

    /**
    * Default constructor.
    */
    Collection() {};
  };


  
  RingBuffer<Collection, 50> collections; /**< Buffered averaged gyro and accleration sensor readings. */
};

#endif // _INERTIAL_FILTER_H_
