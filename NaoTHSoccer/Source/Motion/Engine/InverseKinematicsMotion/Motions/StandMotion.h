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
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugModify.h"

#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/RobotInfo.h>
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Modeling/GroundContactModel.h"
#include "Representations/Modeling/InertialModel.h"
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/CalibrationData.h>
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Infrastructure/CalibrationData.h"
#include "Representations/Infrastructure/FrameInfo.h"

#include <Representations/Modeling/BodyState.h>

//tools
#include "Tools/JointMonitor.h"
#include "Tools/JointOffsets.h"
#include "Tools/StiffnessController.h"

BEGIN_DECLARE_MODULE(StandMotion)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)

  REQUIRE(FrameInfo)

  REQUIRE(RobotInfo)
  REQUIRE(MotionRequest)
  REQUIRE(GroundContactModel)
  REQUIRE(InertialModel)
  REQUIRE(InertialSensorData)
  REQUIRE(GyrometerData)
  REQUIRE(CalibrationData)
  REQUIRE(KinematicChainSensor)
  REQUIRE(MotionStatus)
  PROVIDE(SensorJointData)

  REQUIRE(InverseKinematicsMotionEngineService)

  // body state
  REQUIRE(BodyState)

  PROVIDE(MotionLock)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(StandMotion)

class StandMotion : private StandMotionBase, public IKMotion
{
public:
  enum State {
    GotoStandPose,
    Relax
  };

  State state;
  State lastState;
  time_t state_time;

public:

StandMotion()
  : IKMotion(getInverseKinematicsMotionEngineService(), motion::stand, getMotionLock()),
    
    state(GotoStandPose),
    lastState(Relax),//HACK: need to be different from state (another enum value would require a default case in the switch statement)
    state_time(0),
  
    totalTime(0),
    time(0),
    height(-1000),
    standardStand(true),
    relaxedPoseInitialized(false),
    resetedAfterLifting(false),
    lastFrameInfo(getFrameInfo())
  {
  }


  void execute()
  {
    setCurrentState(motion::running);

    // reset internal stiffness buffer
    // set stiffness from parameters
    for( int i = naoth::JointData::RShoulderRoll; i<naoth::JointData::numOfJoint; i++) {
      stiffness[i] = getEngine().getParameters().stand.stiffness;
    }
    // HACK: turn off the hands
    stiffness[JointData::LHand] = -1;
    stiffness[JointData::RHand] = -1;


    // update joint monitors
    for( int i = naoth::JointData::RShoulderRoll; i <= naoth::JointData::LAnkleRoll; i++) {
        jointMonitors[i].updateMonitor(getMotorJointData().position[i] + jointOffsets[i], getSensorJointData().position[i],getSensorJointData().electricCurrent[i]);
    }

    PLOT("StandMotion:State",state);

    switch(state) 
    {
    case GotoStandPose:
      if(lastState != state) {
        calcStandPose();
        lastState = state;
      }
      
      if(interpolateToStandPose() && state_time > totalTime+static_cast<int>(getRobotInfo().basicTimeStep*100)) {
        if(getMotionRequest().id != getId())  {
          setCurrentState(motion::stopped);
        } else if (!getBodyState().isLiftedUp
                   && getEngine().getParameters().stand.relax.enable
                   && relaxedPoseIsStillOk() ) { //only relax if a valid stand pose is reached
          state = Relax;
        }
      }

      break;

    case Relax:
      // initialize relax on fist execution
      if(lastState != state) {
        relaxedPose = getEngine().getHipFeetPoseBasedOnSensor();
        lastState = state;

        // reset stuff for StandMotion:online_tuning
        jointOffsets.resetOffsets();
        for(int i = 0; i < naoth::JointData::numOfJoint; i++){
            jointMonitors[i].resetAll();
        }
      }

      if ( getMotionRequest().id != getId()
        || getBodyState().isLiftedUp
        || height != getMotionRequest().standHeight // requested height changed
        || standardStand != getMotionRequest().standardStand 
        || !relaxedPoseIsStillOk()) 
      {
        state = GotoStandPose;
      }

      applyPose(relaxedPose);

      if(getEngine().getParameters().stand.relax.jointOffsetTuning.enable){
        tuneJointOffsets();
      }

      if(getEngine().getParameters().stand.relax.stiffnessControl.enable){
        tuneStiffness();
      }
      break;
    };

    // update the time of the current step
    if(lastState != state) {
      state_time = 0;
    } else {
      state_time += getRobotInfo().basicTimeStep;
    }

    // set stiffness from buffer
    for( int i = naoth::JointData::RShoulderRoll; i < naoth::JointData::numOfJoint; i++) {
        getMotorJointData().stiffness[i] = stiffness[i];
    }

    //turnOffStiffnessWhenJointIsOutOfRange();
  }


  void calcStandPose() 
  {
    // initialize standing
    standardStand = getMotionRequest().standardStand;
    height = getMotionRequest().standHeight;

    // init pose
    double comHeight = (height < 0.0) ? getEngine().getParameters().walk.hip.comHeight : getMotionRequest().standHeight;
    comHeight = Math::clamp(comHeight, 160.0, 270.0); // valid range
      
    startPose  = getEngine().getCurrentCoMFeetPose();
    targetPose = getStandPose(comHeight, getEngine().getParameters().stand.hipOffsetX, getEngine().getParameters().stand.bodyPitchOffset, standardStand);

	  // HACK: don't do anything if after walk
    if(getMotionStatus().lastMotion == motion::walk) {
      targetPose = startPose;
    }

    targetPose.localInCoM();
    startPose.localInCoM();

    double speed = getEngine().getParameters().stand.speed;
    double distLeft = (targetPose.feet.left.translation - startPose.feet.left.translation).abs();
    double distRight = (targetPose.feet.right.translation - startPose.feet.right.translation).abs();
    double distMax = std::max(distLeft, distRight);
    totalTime = distMax / speed;
    time = 0;
  }


  bool interpolateToStandPose() 
  {
    // execute the stand motion
    if(totalTime >= 0 && time <= totalTime)
    {
        InverseKinematic::CoMFeetPose p;

        if(totalTime > 0) {
            double k = Math::clamp(0.5*(1.0-cos(time/totalTime*Math::pi)), 0.0, 1.0);
            p = getEngine().interpolate(startPose, targetPose, k);
        } else {
            p = targetPose;
        }

        bool solved = false;
        InverseKinematic::HipFeetPose c = getEngine().controlCenterOfMass(getMotorJointData(), p, solved, false);

        applyPose(c);
        time += getRobotInfo().basicTimeStep;

        return false;
    }

    // done
    return true;
  }


  void applyPose(const InverseKinematic::HipFeetPose& tc) 
  {
    InverseKinematic::HipFeetPose c(tc);
    if(getCalibrationData().calibrated)
    {
      c.localInLeftFoot();

      if(getEngine().getParameters().stand.enableStabilization) {
        getEngine().rotationStabilize(
          getInertialModel(),
          getGyrometerData(),
          getRobotInfo().getBasicTimeStepInSecond(),
          c);
      } else if(getEngine().getParameters().stand.enableStabilizationRC16) {
        getEngine().rotationStabilizeRC16(
          getInertialSensorData(),
          getGyrometerData(),
          getRobotInfo().getBasicTimeStepInSecond(),
          c);
      }
    }

    getEngine().solveHipFeetIK(c);
    getEngine().copyLegJoints(getMotorJointData().position);
  }


  void tuneJointOffsets()
  {
    jointOffsets.setMinimalStep(getEngine().getParameters().stand.relax.jointOffsetTuning.minimalJointStep);

    if(getFrameInfo().getTime() - lastFrameInfo.getTime() > getEngine().getParameters().stand.relax.jointOffsetTuning.deadTime){
      double currents[4] = {jointMonitors[naoth::JointData::LKneePitch].filteredCurrent(), jointMonitors[naoth::JointData::RKneePitch].filteredCurrent(), jointMonitors[naoth::JointData::LAnklePitch].filteredCurrent(), jointMonitors[naoth::JointData::RAnklePitch].filteredCurrent()};

      // determine max
      int max_index = 0;
      for(int i = 1; i < 4; i++){
        if(currents[i] > currents[max_index]){
          max_index = i;
        }
      }

      PLOT("Stand:filteredCurrent:LKneePitch", currents[0]);
      PLOT("Stand:filteredCurrent:RKneePitch", currents[1]);
      PLOT("Stand:filteredCurrent:LAnklePitch",currents[2]);
      PLOT("Stand:filteredCurrent:RAnklePitch",currents[3]);

      // if greater than Threshold then try to tune the offsets
      if(currents[max_index] > getEngine().getParameters().stand.relax.jointOffsetTuning.currentThreshold){
        switch (max_index){
        case 0: //LKnee
          jointOffsets.increaseOffset(naoth::JointData::LKneePitch);
          break;
        case 1: //RKnee
          jointOffsets.increaseOffset(naoth::JointData::RKneePitch);
          break;
        case 2: //LAnklePitch
          jointOffsets.decreaseOffset(naoth::JointData::LAnklePitch);
          break;
        case 3: //RAnklePitch
          jointOffsets.decreaseOffset(naoth::JointData::RAnklePitch);
          break;
        }
      }
      lastFrameInfo = getFrameInfo();
    }

    for( int i = naoth::JointData::RHipYawPitch; i <= naoth::JointData::LAnkleRoll; i++) {
      getMotorJointData().position[i] += jointOffsets[i];
    }
  }

  void tuneStiffness()
  {
      double minAngle = getEngine().getParameters().stand.relax.stiffnessControl.minAngle;
      double maxAngle = getEngine().getParameters().stand.relax.stiffnessControl.maxAngle;
      double minStiff = getEngine().getParameters().stand.relax.stiffnessControl.minStiffness;
      double maxStiff = getEngine().getParameters().stand.relax.stiffnessControl.maxStiffness;

      for( int i = naoth::JointData::RShoulderRoll; i <= naoth::JointData::LAnkleRoll; i++) {
          stiffnessController[i].setMinMaxValues(minAngle,maxAngle,minStiff,maxStiff);
          stiffness[i] = stiffnessController[i].control(jointMonitors[i].getError());
      }

      lastStiffnessUpdate = getFrameInfo();

      PLOT("StandMotion:AverageError:LKneePitch",jointMonitors[naoth::JointData::LKneePitch].getError());
      PLOT("StandMotion:AverageError:RKneePitch",jointMonitors[naoth::JointData::RKneePitch].getError());
      PLOT("StandMotion:Control:LKneePitch",stiffnessController[naoth::JointData::LKneePitch].control(jointMonitors[naoth::JointData::LKneePitch].getError()));
      PLOT("StandMotion:Control:RKneePitch",stiffnessController[naoth::JointData::RKneePitch].control(jointMonitors[naoth::JointData::RKneePitch].getError()));
  }


  bool relaxedPoseIsStillOk() 
  {
    InverseKinematic::CoMFeetPose hipFeetPoseSensor = getEngine().getCoMFeetPoseBasedOnSensor();
    hipFeetPoseSensor.localInLeftFoot();

    // height is the value from the motion request
    InverseKinematic::CoMFeetPose target = getStandPose((height < 0.0) ? getEngine().getParameters().walk.hip.comHeight : getMotionRequest().standHeight, getEngine().getParameters().stand.hipOffsetX, getEngine().getParameters().stand.bodyPitchOffset, standardStand);
    target.localInLeftFoot();

    RotationMatrix hipFeetPoseSensorRot = hipFeetPoseSensor.com.rotation;
    RotationMatrix targetRot = target.com.rotation;

    double rotationErrorX = fabs(hipFeetPoseSensorRot.getXAngle() - targetRot.getXAngle());
    double rotationErrorY = fabs(hipFeetPoseSensorRot.getYAngle() - targetRot.getYAngle());

    PLOT("StandMotion:Deviations:Distance",(hipFeetPoseSensor.com.translation - target.com.translation).abs());
    PLOT("StandMotion:Deviations:RotationY",Math::toDegrees(rotationErrorY));
    PLOT("StandMotion:Deviations:RotationX",Math::toDegrees(rotationErrorX));

    return !( (hipFeetPoseSensor.com.translation - target.com.translation).abs() > getEngine().getParameters().stand.relax.allowedDeviation
              || rotationErrorX > getEngine().getParameters().stand.relax.allowedRotationDeviation
              || rotationErrorY > getEngine().getParameters().stand.relax.allowedRotationDeviation);
  }


private:
  void turnOffStiffnessWhenJointIsOutOfRange()
  {
    const double* pos = getMotorJointData().position;
    double* stiff = getMotorJointData().stiffness;
    // Knee pitch
    if ( pos[naoth::JointData::LKneePitch] > getMotorJointData().max[naoth::JointData::LKneePitch] ) {
      stiff[naoth::JointData::LKneePitch] = -1;
    } else {
      stiff[naoth::JointData::LKneePitch] = getEngine().getParameters().stand.stiffness;
    }
    if ( pos[naoth::JointData::RKneePitch] > getMotorJointData().max[naoth::JointData::RKneePitch] ) {
      stiff[naoth::JointData::RKneePitch] = -1;
    } else {
      stiff[naoth::JointData::RKneePitch] = getEngine().getParameters().stand.stiffness;
    }

    // Ankle pitch
    if ( pos[naoth::JointData::LAnklePitch] < getMotorJointData().min[naoth::JointData::LAnklePitch] ) {
      stiff[naoth::JointData::LAnklePitch] = -1;
    } else {
      stiff[naoth::JointData::LAnklePitch] = getEngine().getParameters().stand.stiffness;
    }
    if ( pos[naoth::JointData::RAnklePitch] < getMotorJointData().min[naoth::JointData::RAnklePitch] ) {
      stiff[naoth::JointData::RAnklePitch] = -1;
    } else {
      stiff[naoth::JointData::RAnklePitch] = getEngine().getParameters().stand.stiffness;
    }
  }//end turnOffStiffnessWhenJointIsOutOfRange

private:

  double totalTime;
  double time;
  double height;
  bool standardStand;

  InverseKinematic::CoMFeetPose targetPose;
  InverseKinematic::CoMFeetPose startPose;


  InverseKinematic::HipFeetPose relaxedPose;
  JointData relaxData;
  bool relaxedPoseInitialized;
  bool isRelaxing;
  bool resetedAfterLifting;

  JointMonitor jointMonitors[naoth::JointData::numOfJoint];

  // used by StandMotion:stiffness_controller
  FrameInfo lastStiffnessUpdate;
  StiffnessController stiffnessController[naoth::JointData::numOfJoint];

  // used by StandMotion:online_tuning
  FrameInfo lastFrameInfo;
  JointOffsets jointOffsets;
  double stiffness[naoth::JointData::numOfJoint];

  };

#endif  /* _StandMotion_H */

