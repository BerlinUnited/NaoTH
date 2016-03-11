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

#include <Tools/DataStructures/RingBufferWithSum.h>

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
    relaxedPoseInitialized(false),
    relaxedJointsValid(false),
    lastFrameInfo(getFrameInfo()),
    relaxedMotorJointsValid(false),
    alpha(0.99),
    beta(0.5)
  {
    DEBUG_REQUEST_REGISTER("StandMotion:relax_joints", "set snsor joint data to motor joint data", false);
    DEBUG_REQUEST_REGISTER("StandMotion:relax_joints_loop", "set snsor joint data to motor joint data", false);
    DEBUG_REQUEST_REGISTER("StandMotion:relax_joints_loop_each_second", "set snsor joint data to motor joint data", false);
    DEBUG_REQUEST_REGISTER("StandMotion:relax_init", "set snsor joint data to motor joint data", false);
    DEBUG_REQUEST_REGISTER("StandMotion:relax_joints_continuously", "continuously relax the joints", false);
    DEBUG_REQUEST_REGISTER("StandMotion:use_filtered_motor_joint_commands", "continuously relax the joints using a simple filter", false);
    DEBUG_REQUEST_REGISTER("StandMotion:add_sine_to_motor_commands", "add a sine to the motor commands while the robot is standing", false);
    DEBUG_REQUEST_REGISTER("StandMotion:add_sine_to_hip","add a sine to the hip-feet-pose while the robot is standing", false);
    DEBUG_REQUEST_REGISTER("StandMotion:change_stiffness","set the stiffness for each joint with modify", false);

    // init sine
    for(int i = naoth::JointData::RHipYawPitch; i <= naoth::JointData::LAnkleRoll; i++) {
        sine[i] = Sine(Math::fromDegrees(0.08),2*getRobotInfo().getBasicTimeStepInSecond());
        amplitudes[i] = 0.08;
        periods[i] = 2*getRobotInfo().getBasicTimeStepInSecond();
    }

    // init hip sine
    amplitudeX = 5;
    periodX    = 5;
    phaseX     = 0;
    hipSineX.setAmplitude(amplitudeX);
    hipSineX.setPeriod(periodX);

    amplitudeY = 5;
    periodY    = 5;
    phaseY     = 0;
    hipSineY.setAmplitude(amplitudeY);
    hipSineY.setPeriod(periodY);

    // init stiffness
    for(int i = naoth::JointData::RHipYawPitch; i <= naoth::JointData::LAnkleRoll; i++) {
        stiffness[i] = 0.7;
    }
  }

  void calculateTrajectory(const MotionRequest& motionRequest)
  {
    // standing
    if ( isStopped()
        || height != motionRequest.standHeight // requested height changed
        || standardStand != motionRequest.standardStand ) 
    {
      standardStand = motionRequest.standardStand;
      height = motionRequest.standHeight;

      // init pose
      double comHeight = (height < 0.0) ? getEngine().getParameters().walk.hip.comHeight : motionRequest.standHeight;
      comHeight = Math::clamp(comHeight, 160.0, 270.0); // valid range
      
      startPose = getEngine().getCurrentCoMFeetPose();
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

      // set stiffness
      for( int i = naoth::JointData::RShoulderRoll; i<naoth::JointData::numOfJoint; i++) {
        getMotorJointData().stiffness[i] = getEngine().getParameters().stand.stiffness;
      }
      // HACK: turn off the hands
      getMotorJointData().stiffness[JointData::LHand] = -1;
      getMotorJointData().stiffness[JointData::RHand] = -1;
    }
  }//end calculateTrajectory


  void execute()
  {
    calculateTrajectory(getMotionRequest());

    InverseKinematic::HipFeetPose c;

    isRelaxing = false;

    if(time > totalTime + getRobotInfo().basicTimeStep*10) { // the robot is standing and tries to save energy due to relaxing its joints

      isRelaxing = true;

      if(!relaxedPoseInitialized) {
        relaxedPoseInitialized = true;
        relaxedPose = getEngine().getHipFeetPoseBasedOnSensor();
      }

      DEBUG_REQUEST("StandMotion:relax_init",
        relaxData = getSensorJointData();
        relaxedPose = getEngine().getHipFeetPoseBasedOnSensor();
        relaxedPoseInitialized = true;
      );

      c = relaxedPose;

      InverseKinematic::HipFeetPose hipFeetPoseSensor = getEngine().getHipFeetPoseBasedOnSensor();
      hipFeetPoseSensor.localInLeftFoot();

      InverseKinematic::HipFeetPose target = relaxedPose;
      target.localInLeftFoot();

      if((hipFeetPoseSensor.hip.translation - target.hip.translation).abs() > 10 /*5*/) {
          isRelaxing = false; //because the stand motion will be restarted
          relaxedPoseInitialized = false;
          relaxedJointsValid = false;
          relaxedMotorJointsValid = false;
          setCurrentState(motion::stopped);
          calculateTrajectory(getMotionRequest());

          totalTime += 1000;
      }

      PLOT("Stand:hipErrorToTarget",(hipFeetPoseSensor.hip.translation - target.hip.translation).abs());
    }

    if(totalTime > 0 && time <= totalTime + getRobotInfo().basicTimeStep*10)
    {
      double k = Math::clamp(0.5*(1.0-cos(time/totalTime*Math::pi)), 0.0, 1.0);
      InverseKinematic::CoMFeetPose p = getEngine().interpolate(startPose, targetPose, k);

      bool solved = false;
      c = getEngine().controlCenterOfMass(getMotorJointData(), p, solved, false);
    }
    
    /*
    InverseKinematic::HipFeetPose c;
    c.hip = p.com;
    c.hip.translation = Vector3d();
    c.feet = p.feet;

    getEngine().controlCenterOfMassCool(getMotorJointData(), p, c, true, solved, false);
    */

    if(getCalibrationData().calibrated && getEngine().getParameters().stand.enableStabilization)
    {
      /*
      getEngine().rotationStabilize(
        getRobotInfo(),
        getGroundContactModel(),
        getInertialSensorData(),
        c.hip);*/

      c.localInLeftFoot();
      getEngine().rotationStabilize(
        getInertialModel(),
        getGyrometerData(),
        getRobotInfo().getBasicTimeStepInSecond(),
        c);
    }

    DEBUG_REQUEST("StandMotion:add_sine_to_hip",

        c.localInLeftFoot();
        Vector3d x_direction = c.hip.rotation*Vector3d(1,0,0);
        Vector3d y_direction = c.hip.rotation*Vector3d(0,1,0);

        MODIFY("StandMotion:hip_sine:X:amplitude",amplitudeX);
        MODIFY("StandMotion:hip_sine:X:period",periodX);
        MODIFY("StandMotion:hip_sine:X:phase",phaseX);
        hipSineX.setAmplitude(amplitudeX);
        hipSineX.setPeriod(periodX);
        hipSineX.setPhase(phaseX);

        MODIFY("StandMotion:hip_sine:Y:amplitude",amplitudeY);
        MODIFY("StandMotion:hip_sine:Y:period",periodY);
        MODIFY("StandMotion:hip_sine:Y:phase",phaseY);
        hipSineY.setAmplitude(amplitudeY);
        hipSineY.setPeriod(periodY);
        hipSineY.setPhase(phaseY);

        c.hip.translation += x_direction * hipSineX(getFrameInfo().getTimeInSeconds());
        c.hip.translation += y_direction * hipSineY(getFrameInfo().getTimeInSeconds());
    );

    getEngine().solveHipFeetIK(c);
    getEngine().copyLegJoints(getMotorJointData().position);

    DEBUG_REQUEST("StandMotion:relax_joints_loop_each_second",
      if (isRelaxing && (getFrameInfo().getTime() - lastFrameInfo.getTime() > 1000)) {
        lastFrameInfo = getFrameInfo();
        relaxedJoints = getSensorJointData();
        relaxedJointsValid = true;
      }

      if(relaxedJointsValid) {
          for( int i = naoth::JointData::RHipYawPitch; i<naoth::JointData::LAnkleRoll; i++) {
            getMotorJointData().position[i] = relaxedJoints.position[i];
          }
      }
    );

    DEBUG_REQUEST("StandMotion:relax_joints_continuously",
      if(isRelaxing){
        if(!relaxedMotorJointsValid) {
          relaxedMotorJointsValid = true;
          relaxedMotorJoints      = getMotorJointData();
        }

        MODIFY("StandMotion:relax_joints_continuously:alpha",alpha);
        for( int i = naoth::JointData::RHipYawPitch; i <= naoth::JointData::LAnkleRoll; i++) {
            getMotorJointData().position[i] = alpha * relaxedMotorJoints.position[i] + (1 - alpha) * getSensorJointData().position[i];
        }

        relaxedMotorJoints = getMotorJointData();
      }
    );

    DEBUG_REQUEST("StandMotion:use_filtered_motor_joint_commands",
      if (isRelaxing) {
        MODIFY("StandMotion:motor_filter:k_i",beta);
        for( int i = naoth::JointData::RHipYawPitch; i <= naoth::JointData::LAnkleRoll; i++) {
            filter[i].updateFilter(getMotorJointData().position[i],getSensorJointData().position[i]);
            filter[i].setK_i(beta);

            getMotorJointData().position[i] = getMotorJointData().position[i] + filter[i].control();
        }
      } else {
        for( int i = naoth::JointData::RHipYawPitch; i<naoth::JointData::LAnkleRoll; i++) {
          filter[i].resetFilter();
        }
      }
    );

    DEBUG_REQUEST("StandMotion:add_sine_to_motor_commands",
      if (isRelaxing) {

        MODIFY("StandMotion:sine:LHipYawPitch:amplitude[°]",amplitudes[naoth::JointData::LHipYawPitch]);
        MODIFY("StandMotion:sine:LHipYawPitch:period[s]", periods[naoth::JointData::LHipYawPitch]);
        MODIFY("StandMotion:sine:RHipPitch:amplitude[°]",amplitudes[naoth::JointData::RHipPitch]);
        MODIFY("StandMotion:sine:RHipPitch:period[s]", periods[naoth::JointData::RHipPitch]);
        MODIFY("StandMotion:sine:LHipPitch:amplitude[°]",amplitudes[naoth::JointData::LHipPitch]);
        MODIFY("StandMotion:sine:LHipPitch:period[s]", periods[naoth::JointData::LHipPitch]);
        MODIFY("StandMotion:sine:RHipRoll:amplitude[°]",amplitudes[naoth::JointData::RHipRoll]);
        MODIFY("StandMotion:sine:RHipRoll:period[s]", periods[naoth::JointData::RHipRoll]);
        MODIFY("StandMotion:sine:LHipRoll:amplitude[°]",amplitudes[naoth::JointData::LHipRoll]);
        MODIFY("StandMotion:sine:LHipRoll:period[s]", periods[naoth::JointData::LHipRoll]);
        MODIFY("StandMotion:sine:RKneePitch:amplitude[°]",amplitudes[naoth::JointData::RKneePitch]);
        MODIFY("StandMotion:sine:RKneePitch:period[s]", periods[naoth::JointData::RKneePitch]);
        MODIFY("StandMotion:sine:LKneePitch:amplitude[°]",amplitudes[naoth::JointData::LKneePitch]);
        MODIFY("StandMotion:sine:LKneePitch:period[s]", periods[naoth::JointData::LKneePitch]);
        MODIFY("StandMotion:sine:RAnklePitch:amplitude[°]",amplitudes[naoth::JointData::RAnklePitch]);
        MODIFY("StandMotion:sine:RAnklePitch:period[s]", periods[naoth::JointData::RAnklePitch]);
        MODIFY("StandMotion:sine:LAnklePitch:amplitude[°]",amplitudes[naoth::JointData::LAnklePitch]);
        MODIFY("StandMotion:sine:LAnklePitch:period[s]", periods[naoth::JointData::LAnklePitch]);
        MODIFY("StandMotion:sine:RAnkleRoll:amplitude[°]",amplitudes[naoth::JointData::RAnkleRoll]);
        MODIFY("StandMotion:sine:RAnkleRoll:period[s]", periods[naoth::JointData::RAnkleRoll]);
        MODIFY("StandMotion:sine:LAnkleRoll:amplitude[°]",amplitudes[naoth::JointData::LAnkleRoll]);
        MODIFY("StandMotion:sine:LAnkleRoll:period[s]", periods[naoth::JointData::LAnkleRoll]);

        for( int i = naoth::JointData::RHipYawPitch; i <= naoth::JointData::LAnkleRoll; i++) {
            sine[i].setAmplitude(Math::fromDegrees(amplitudes[i]));
            sine[i].setPeriod(periods[i]);
            getMotorJointData().position[i] = getMotorJointData().position[i] + sine[i](getFrameInfo().getTimeInSeconds());
        }
      } else {
        for( int i = naoth::JointData::RHipYawPitch; i <= naoth::JointData::LAnkleRoll; i++) {
          filter[i].resetFilter();
        }
      }
    );


    DEBUG_REQUEST("StandMotion:relax_joints_loop",
      for( int i = naoth::JointData::RHipYawPitch; i <= naoth::JointData::LAnkleRoll; i++) {
        getMotorJointData().position[i] = getSensorJointData().position[i];
      }
    );

    DEBUG_REQUEST("StandMotion:relax_joints",
      for( int i = naoth::JointData::RHipYawPitch; i <= naoth::JointData::LAnkleRoll; i++) {
        getMotorJointData().position[i] = relaxData.position[i];
      }
    );

    /*
    getEngine().gotoArms(
        getMotionStatus(),
        getInertialModel(),
        getRobotInfo(),
        c, getMotorJointData().position);
    */
    PLOT("Stand:hip:x",c.hip.translation.x);
    PLOT("Stand:hip:y",c.hip.translation.y);
    PLOT("Stand:hip:z",c.hip.translation.z);
    PLOT("Stand:time:time", time);
    PLOT("Stand:time:totalTime", totalTime);

    time += getRobotInfo().basicTimeStep;

    turnOffStiffnessWhenJointIsOutOfRange();

    DEBUG_REQUEST("StandMotion:change_stiffness",

      if (isRelaxing) {
        MODIFY("StandMotion:stiffness:LHipYawPitch",stiffness[naoth::JointData::LHipYawPitch]);
        MODIFY("StandMotion:stiffness:RHipPitch",stiffness[naoth::JointData::RHipPitch]);
        MODIFY("StandMotion:stiffness:LHipPitch",stiffness[naoth::JointData::LHipPitch]);
        MODIFY("StandMotion:stiffness:RHipRoll",stiffness[naoth::JointData::RHipRoll]);
        MODIFY("StandMotion:stiffness:LHipRoll",stiffness[naoth::JointData::LHipRoll]);
        MODIFY("StandMotion:stiffness:RKneePitch",stiffness[naoth::JointData::RKneePitch]);
        MODIFY("StandMotion:stiffness:LKneePitch",stiffness[naoth::JointData::LKneePitch]);
        MODIFY("StandMotion:stiffness:RAnklePitch",stiffness[naoth::JointData::RAnklePitch]);
        MODIFY("StandMotion:stiffness:LAnklePitch",stiffness[naoth::JointData::LAnklePitch]);
        MODIFY("StandMotion:stiffness:RAnkleRoll",stiffness[naoth::JointData::RAnkleRoll]);
        MODIFY("StandMotion:stiffness:LAnkleRoll",stiffness[naoth::JointData::LAnkleRoll]);

        for( int i = naoth::JointData::RHipYawPitch; i <= naoth::JointData::LAnkleRoll; i++) {
          getMotorJointData().stiffness[i] = stiffness[i];
        }
      }
    );

    
    if ( time >= totalTime && getMotionRequest().id != getId() ) {
      setCurrentState(motion::stopped);
      relaxedJointsValid      = false;
      relaxedPoseInitialized  = false;
      relaxedMotorJointsValid = false;
      return;
    } else {
      setCurrentState(motion::running);
    }
  }//end execute

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

  // used by relax_joints_loop_each_second
  JointData relaxedJoints;
  bool relaxedJointsValid;
  FrameInfo lastFrameInfo;

  // used by relax_joints_continuously
  JointData relaxedMotorJoints;
  bool relaxedMotorJointsValid;
  double alpha;

  // used by use_filtered_motor_joint_commands
  class jointFilter
  {
  public:
      jointFilter() {}

      void updateFilter(double motorData, double sensorData) {
          motorJointDataBuffer.add(motorData);

          if(motorJointDataBuffer.isFull()){
              motorToSensorError.add(sensorData - motorJointDataBuffer.first());
          }
      }

      double control() {
        if(motorToSensorError.isFull()){
          return k_i * motorToSensorError.getAverage();
        } else {
          return 0;
        }
      }

      void setK_i(double newK_i) {
          k_i = newK_i;
      }

      void resetFilter() {
          motorJointDataBuffer.clear();
          motorToSensorError.clear();
      }

      double getLatestError() {
          if(motorToSensorError.size() >= 1)
            return motorToSensorError.last();
          return 0;
      }

  private:
      RingBuffer<double,4> motorJointDataBuffer;
      RingBufferWithSum<double,100> motorToSensorError;
      double k_i;
  };

  jointFilter filter[naoth::JointData::numOfJoint];
  double beta;

  // used by StandMotion:add_sine_to_motor_commands
  class Sine{
  public:
      Sine():
        amplitude(0),
        period(1),
        phase(0)
      {
      }

      Sine(double amplitude, double period):
          amplitude(amplitude),
          period(period)
      {}

      double operator() (double time){
          return amplitude * sin(2*M_PI/period * time + phase);
      }

      void setAmplitude(double a) {
          amplitude = a;
      }

      void setPeriod(double T){
          period = T;
      }

      void setPhase(double phi){
          phase = phi;
      }

  private:
    double amplitude; // [rad]
    double period;    // [s]
    double phase;     // [rad]
  };

  Sine sine[naoth::JointData::numOfJoint];
  double amplitudes[naoth::JointData::numOfJoint]; // [°]
  double periods[naoth::JointData::numOfJoint]; // [s]

  // used by StandMotion:add_sine_to_hip
  Sine hipSineX;
  double amplitudeX;
  double periodX;
  double phaseX;
  Sine hipSineY;
  double amplitudeY;
  double periodY;
  double phaseY;

  // used by StandMotion:change_stiffness
  double stiffness[naoth::JointData::numOfJoint];
};

#endif  /* _StandMotion_H */

