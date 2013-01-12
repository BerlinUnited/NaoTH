/**
* @file StandMotion.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*
* the stand motion goes from current pose to the default stand pose.
* running - this motion is cyclic and there is allways running when the motion
*           request is set to stand
* stopped - the motion stopps when the motion request is not stand and the 
*           default stand pose has been reached
* 
*/

#ifndef _StandMotion_H
#define _StandMotion_H

#include "IKMotion.h"
#include "Tools/Debug/DebugBufferedOutput.h"

#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/RobotInfo.h>
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Modeling/GroundContactModel.h"
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/JointData.h>

BEGIN_DECLARE_MODULE(StandMotion)
  REQUIRE(RobotInfo)
  REQUIRE(MotionRequest)
  REQUIRE(GroundContactModel)
  REQUIRE(InertialSensorData)

  REQUIRE(InverseKinematicsMotionEngineService)

  PROVIDE(MotionLock)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(StandMotion)

class StandMotion : private StandMotionBase, public IKMotion
{
public:

  StandMotion()
  : IKMotion(getInverseKinematicsMotionEngineService(), motion::stand, getMotionLock()),
    totalTime(0),
    time(0),
    height(-1000),
    standardStand(true),
    stiffness(0.7)
  {
  }

  void calculateTrajectory(const MotionRequest& motionRequest)
  {
    // standing
    if ( isStopped()
        || abs(height - motionRequest.standHeight) > 1
        || standardStand != motionRequest.standardStand ) {
      standardStand = motionRequest.standardStand;
      // init pose
      height = motionRequest.standHeight;
      double comHeight = (motionRequest.standHeight < 0.0) ? getEngine().getParameters().walk.comHeight : motionRequest.standHeight;
      comHeight = Math::clamp(comHeight, 160.0, 270.0); // valid range
      startPose = getEngine().getCurrentCoMFeetPose();
      targetPose = getStandPose(comHeight, standardStand);

      targetPose.localInCoM();
      startPose.localInCoM();

      double speed = getEngine().getParameters().stand.speed;
      double distLeft = (targetPose.feet.left.translation - startPose.feet.left.translation).abs();
      double distRight = (targetPose.feet.right.translation - startPose.feet.right.translation).abs();
      double distMax = (distLeft > distRight) ? distLeft : distRight;
      totalTime = distMax / speed;
      time = 0;

      // set stiffness
      for( int i=naoth::JointData::RShoulderRoll; i<naoth::JointData::numOfJoint; i++)
      {
        getMotorJointData().stiffness[i] = stiffness;
      }
    }
  }//end calculateTrajectory


  void execute()
  {
    if ( time > totalTime && getMotionRequest().id != getId() )
    {
      setCurrentState(motion::stopped);
      return;
    }

    calculateTrajectory(getMotionRequest());

    time += getRobotInfo().basicTimeStep;
    double k = std::min(time / totalTime, 1.0);

    InverseKinematic::CoMFeetPose p = getEngine().interpolate(startPose, targetPose, k);
    bool solved = false;
    InverseKinematic::HipFeetPose c = getEngine().controlCenterOfMass(getMotorJointData(), p, solved, false);

    if(getEngine().getParameters().stand.enableStabilization)
      getEngine().rotationStabilize(
        getRobotInfo(),
        getGroundContactModel(),
        getInertialSensorData(),
        c.hip, c.feet.left, c.feet.right);

    getEngine().solveHipFeetIK(c);
    getEngine().copyLegJoints(getMotorJointData().position);
    getEngine().autoArms(getRobotInfo(), c, getMotorJointData().position);


    PLOT("Stand:hip:x",c.hip.translation.x);
    PLOT("Stand:hip:y",c.hip.translation.y);
    PLOT("Stand:hip:z",c.hip.translation.z);


    //if(theParameters.stand.stabilizeNeural)
    //  theEngine.feetStabilize(getMotorJointData().position);

    turnOffStiffnessWhenJointIsOutOfRange();
    setCurrentState(motion::running);
  }//end execute

private:
  void turnOffStiffnessWhenJointIsOutOfRange()
  {
    const double* pos = getMotorJointData().position;
    double* stiff = getMotorJointData().stiffness;
    // Knee pitch
    if ( pos[naoth::JointData::LKneePitch] > getMotorJointData().max[naoth::JointData::LKneePitch] )
    {
      stiff[naoth::JointData::LKneePitch] = -1;
    }
    else
    {
      stiff[naoth::JointData::LKneePitch] = stiffness;
    }
    if ( pos[naoth::JointData::RKneePitch] > getMotorJointData().max[naoth::JointData::RKneePitch] )
    {
      stiff[naoth::JointData::RKneePitch] = -1;
    }
    else
    {
      stiff[naoth::JointData::RKneePitch] = stiffness;
    }

    // Ankle pitch
    if ( pos[naoth::JointData::LAnklePitch] < getMotorJointData().min[naoth::JointData::LAnklePitch] )
    {
      stiff[naoth::JointData::LAnklePitch] = -1;
    }
    else
    {
      stiff[naoth::JointData::LAnklePitch] = stiffness;
    }
    if ( pos[naoth::JointData::RAnklePitch] < getMotorJointData().min[naoth::JointData::RAnklePitch] )
    {
      stiff[naoth::JointData::RAnklePitch] = -1;
    }
    else
    {
      stiff[naoth::JointData::RAnklePitch] = stiffness;
    }
  }

private:
  double totalTime;
  double time;
  double height;
  bool standardStand;
  InverseKinematic::CoMFeetPose targetPose;
  InverseKinematic::CoMFeetPose startPose;
  double stiffness;
};

#endif  /* _StandMotion_H */

