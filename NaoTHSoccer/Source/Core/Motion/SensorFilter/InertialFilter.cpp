/**
 * @file InertialFilter.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#include "InertialFilter.h"

using namespace naoth;

InertialFilter::InertialFilter(const MotionBlackBoard& bb, Vector2d& offset, bool& offsetUpdated):
  theBlackBoard(bb),
  sensorData(bb.theInertialSensorData),
  theOffset(offset),
  theOffsetUpdated(offsetUpdated),
  calibrateNum(0),
  max_offet(Math::fromDegrees(6))
{
}

InertialPercept InertialFilter::filter()
{
  calibrate();

  InertialPercept result;
  result.data = sensorData.data + theOffset;

  return result;
}

void InertialFilter::calibrate()
{
  if ( theBlackBoard.theMotionStatus.currentMotion == motion::stand
      && !intentionallyMoving() )
  {
    // stand and not moving
    if ( theBlackBoard.theSupportPolygon.mode & (SupportPolygon::DOUBLE | SupportPolygon::DOUBLE_LEFT | SupportPolygon::DOUBLE) )
    {
      // both feet on the ground
      Pose3D foot = theBlackBoard.theKinematicChain.theLinks[KinematicChain::LFoot].M;
      const Pose3D& body = theBlackBoard.theKinematicChain.theLinks[KinematicChain::Torso].M;
      Pose3D bodyInFoot = foot.invert() * body;
      Vector2d expectedAngle;
      expectedAngle.x = bodyInFoot.rotation.getXAngle();
      expectedAngle.y = bodyInFoot.rotation.getYAngle();
      Vector2d diff = sensorData.data - expectedAngle;
      if ( abs(diff.x) < max_offet && abs(diff.y) < max_offet )
      {
        // do calibrate
        theNewOffset = (theNewOffset*calibrateNum - diff) / (calibrateNum+1);
        calibrateNum++;
        if ( calibrateNum % 50 == 0 )
        {
          // every 50 datas
          theOffset = theNewOffset;
          theOffsetUpdated = true;
        }
        return;
      }
    }
  }

  calibrateNum = 0;
}

// check all request joints' speed, return true if all joints are almost not moving
bool InertialFilter::intentionallyMoving()
{
  const double* jointSpeed = theBlackBoard.theMotorJointData.dp;
  const double* stiffness = theBlackBoard.theMotorJointData.stiffness;
  const double min_speed = Math::fromDegrees(1); // degree per second
  const double min_stiffness = 0.05;
  for( int i=0; i<JointData::numOfJoint; i++)
  {
    if ( stiffness[i] > min_stiffness && abs(jointSpeed[i]) > min_speed )
    {
      return true;
    }
  }

  return false;
}
