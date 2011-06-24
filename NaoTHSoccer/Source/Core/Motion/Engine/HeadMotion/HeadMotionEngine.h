/**
 * @file HeadMotionEngine.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author Oliver Welter
 *
 * Created on 27. Januar 2009, 16:43
 */

#ifndef _HEADMOTIONENGINE_H
#define	_HEADMOTIONENGINE_H

#include <Tools/Math/Vector2.h>
#include <PlatformInterface/Platform.h>
#include "Motion/MotionBlackBoard.h"

class HeadMotionEngine
{
public:
  /** constructor */
  HeadMotionEngine();
  ~HeadMotionEngine(){};

  void execute();

private:
  const MotionBlackBoard& theBlackBoard;

  naoth::MotorJointData& theMotorJointData;
  naoth::JointData theJointData;

  MotionStatus& theMotionStatus;
  KinematicChain theKinematicChain;
  

  bool trajectoryHeadMove(const std::vector<Vector3<double> >& points);
  void gotoPointOnTheGround(const Vector2<double>& target);

  void lookStraightAhead();
  void lookStraightAheadWithStabilization();
  void lookAtPoint();
  void simpleLookAtPoint();

  void hold();
  void search();
  void randomSearch();


  void moveByAngle(const Vector2<double>& target);
  void gotoAngle(const Vector2<double>& target);

  void lookAtWorldPoint(const Vector3<double>& target);
  void lookAtWorldPointSimple(const Vector3<double>& target);

  Vector3<double> g(double yaw, double pitch, const Vector3<double>& pointInWorld);
};

#endif	/* _HEADMOTIONENGINE_H */

