/**
 * @file InertialFilter.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef _INERTIAL_FILTER_H_
#define _INERTIAL_FILTER_H_

#include "Motion/MotionBlackBoard.h"

class InertialFilter
{
public:
  InertialFilter(const MotionBlackBoard& bb);

  InertialPercept filte();

private:
  void calibrate();

  bool intentionallyMoving();

private:
  const MotionBlackBoard& theBlackBoard;
  const naoth::InertialSensorData& sensorData;

  Vector2d theOffset;
  Vector2d theNewOffset;
  unsigned int calibrateNum;
  const double max_offet;
};

#endif // _INERTIAL_FILTER_H_
