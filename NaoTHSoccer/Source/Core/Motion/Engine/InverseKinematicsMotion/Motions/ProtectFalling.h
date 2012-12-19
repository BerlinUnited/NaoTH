/**
* @file ProtectFalling.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Remember the pose in fist frame, then apply stablization,
* and also decrease stiffness according to body rotation
*/

#ifndef PROTECTFALLING_H
#define PROTECTFALLING_H

#include "IKMotion.h"

#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/RobotInfo.h>
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Modeling/GroundContactModel.h"
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/JointData.h>


BEGIN_DECLARE_MODULE(ProtectFalling)
  REQUIRE(RobotInfo)
  REQUIRE(MotionRequest)
  REQUIRE(SensorJointData)
  REQUIRE(GroundContactModel)
  REQUIRE(InertialSensorData)

  REQUIRE(InverseKinematicsMotionEngineService)

  PROVIDE(MotionLock)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(ProtectFalling)

class ProtectFalling: private ProtectFallingBase, public IKMotion
{
public:

  ProtectFalling()
  : IKMotion(getInverseKinematicsMotionEngineService(), motion::protect_falling, getMotionLock())
  {
      for(int i=0; i<naoth::JointData::numOfJoint; i++)
      {
        keepStiffness[i] = 0.0;
      }
  }

  void execute()
  {
    if ( getMotionRequest().id != getId() )
    {
      // exit at any time
      setCurrentState(motion::stopped);
      return;
    }

    if ( isStopped() )
    {
      // remember the pose
      keepPose = getEngine().getCurrentHipFeetPose();
      for(int i=0; i<naoth::JointData::numOfJoint; i++)
      {
        keepStiffness[i] = getSensorJointData().stiffness[i];
      }
    }

    InverseKinematic::HipFeetPose c = keepPose;

    getEngine().rotationStabilize(
      getRobotInfo(), 
      getGroundContactModel(),
      getInertialSensorData(),
      c.hip, c.feet.left, c.feet.right);

    getEngine().solveHipFeetIK(c);
    getEngine().copyLegJoints(getMotorJointData().position);
    decreaseStiffness();
    setCurrentState(motion::running);
  }

private:
  /* decrease stiffness according to body rotation */
  void decreaseStiffness()
  {
    const Vector2d& is = getInertialSensorData().data;
    double angle = max( abs(is.x), abs(is.y) );

    // limit the range
    angle = Math::clamp(angle, 0.0, Math::pi_2);

    double k = 1.5 * cos(angle) - 0.5;
    for( int i=0; i<naoth::JointData::numOfJoint; i++)
    {
      if ( k < 0 )
      {
        getMotorJointData().stiffness[i] = 0; // don't turn the motor off
      }
      else
      {
        getMotorJointData().stiffness[i] = k * keepStiffness[i];
      }
    }
  }

private:
    InverseKinematic::HipFeetPose keepPose;
    double keepStiffness[naoth::JointData::numOfJoint];
};

#endif // PROTECTFALLING_H
