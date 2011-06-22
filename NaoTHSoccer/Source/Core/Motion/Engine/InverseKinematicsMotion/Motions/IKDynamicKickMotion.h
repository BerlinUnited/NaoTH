/**
* @file IKDynamicKickMotion.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class IKDynamicKickMotion
*/

#ifndef __IKDynamicKickMotion_h__
#define __IKDynamicKickMotion_h__

// kinematic tools
#include "IKMotion.h"
#include "IKParameters.h"
#include "ReachibilityGrid.h"


class KickPose
{
public:
  InverseKinematic::CoMFeetPose pose;
  unsigned int time;

  KickPose()
    : time(0)
  {
  }

  KickPose(const InverseKinematic::CoMFeetPose& pose)
    : pose(pose),
      time(0)
  {
  }
};

template<class OWNER, class ELEMENT>
class Trajectory
{
  // HACK
  friend class IKDynamicKickMotion;

private:
  std::list<ELEMENT> trajectory;

  void push_back(const ELEMENT& pose)
  {
    trajectory.push_back(pose);
  }

  void pop_front()
  {
    trajectory.pop_front();
  }

  ELEMENT& non_const_front()
  {
    ASSERT(!trajectory.empty());
    return trajectory.front();
  }

  ELEMENT& non_const_back()
  {
    ASSERT(!trajectory.empty());
    return trajectory.back();
  }

  void clear()
  {
    trajectory.clear();
  }


public:
  // FIXME: make me private
  // clear the trajectory up to one element

  void clear_but_one()
  {
    while (trajectory.size() > 1) {
      trajectory.pop_back();
    }
    ASSERT(!trajectory.empty());
  }

  const ELEMENT& back() const
  {
    ASSERT(!trajectory.empty());
    return trajectory.back();
  }

  const ELEMENT& front() const
  {
    ASSERT(!trajectory.empty());
    return trajectory.front();
  }

  size_t size() const
  {
    return trajectory.size();
  }

  bool empty() const
  {
    return trajectory.empty();
  }
};


class IKDynamicKickMotion: public IKMotion
{
private:
  // local transformated reachability grid
  // TODO: make it global?
  ReachibilityGrid basicReachibilityGrid;
  TransformedReachibilityGrid reachibilityGrid;

  const IKParameters::KickParameters& theParameters;

  // needed to finish prekick
  int numberOfPreKickSteps;

  KickRequest::KickFootID theKickingFoot;

  KickPose currentPose;
  Trajectory<IKDynamicKickMotion,KickPose> trajectory;
 
  bool wrap_up_made;

  // --- begin --- state machine
  enum KickState
  {
    kick_prepare,
    kick_retract,
    kick_execute,
    kick_wrap_up
  };

  KickState kickState;
  KickState oldKickState;
  bool action_done;
  unsigned int state_start_time;

  void action_prepare();
  void action_retract(const Pose3D& kickPose);
  void action_execute(const Pose3D& kickPose);
  void action_wrap_up();

  // --- end --- state machine


  // returns the state of the last element in the trajectory
  bool isRequested(const MotionRequest& motionRequest) const;
  //IKMEState currentTrajectoryState() const;
  //IKMEState preKickState() const;
  //IKMEState initialState() const;
  void stopKick();
  inline double getStandHeight() const;
  void localInStandFoot( KickPose& pose ) const;


  // tool methods
  Vector3<double> adjustKickPoseForKickDirection(const Pose3D& orgKickPoint) const;
  Pose3D calculateKickPoseInSupportFootCoordinates(const Vector3<double>& kickPoint, double direction) const;
  Pose3D clampKickPoseInReachableSpace(const Pose3D& pose) const;
  Pose3D calculatePreparationPose(const Pose3D& kickPose) const;
  
  double retractionValue(const Pose3D& kickPose, const Vector3<double>& pointCoord) const;
  Vector3<double> calculateTheLoadPoint(const Pose3D& kickPose) const;
  Vector3<double> calculateKickPointProjection(const Pose3D& kickPose) const;


  // debug methods
  void drawReachabilityGrid(const Pose3D& kickPose, bool draw2D, bool draw3D) const;

public:

  IKDynamicKickMotion();
  ~IKDynamicKickMotion();

  virtual void execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/);
  virtual void calculateTrajectory(const MotionRequest& motionRequest);

};

#endif // __IKDynamicKickMotion_h__
