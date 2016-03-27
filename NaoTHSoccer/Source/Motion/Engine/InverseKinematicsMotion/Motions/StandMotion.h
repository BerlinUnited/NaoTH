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
    lastFrameInfo(getFrameInfo())
  {
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

      c = relaxedPose;

      InverseKinematic::HipFeetPose hipFeetPoseSensor = getEngine().getHipFeetPoseBasedOnSensor();
      hipFeetPoseSensor.localInLeftFoot();

      InverseKinematic::HipFeetPose target = relaxedPose;
      target.localInLeftFoot();

      if((hipFeetPoseSensor.hip.translation - target.hip.translation).abs() > 5) {
          isRelaxing = false; //because the stand motion will be restarted
          relaxedPoseInitialized = false;
          setCurrentState(motion::stopped);
          calculateTrajectory(getMotionRequest());

          // reset stuff for StandMotion:online_tuning
          jointOffsets.resetOffsets();
          for(int i = 0; i < naoth::JointData::numOfJoint; i++){
            jointMonitors[i].resetAll();
          }

          totalTime += 1000;
      }
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

    getEngine().solveHipFeetIK(c);
    getEngine().copyLegJoints(getMotorJointData().position);

    // update joint monitors
    for( int i = naoth::JointData::RHipYawPitch; i <= naoth::JointData::LAnkleRoll; i++) {
        jointMonitors[i].updateMonitor(getMotorJointData().position[i], getSensorJointData().position[i],getSensorJointData().electricCurrent[i]);
    }
 
    // control the joint offsets (online tuning) for the knee and ankle pitchs
    if (isRelaxing) {
        if(getFrameInfo().getTime() - lastFrameInfo.getTime() > 1000){
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

            // if greater than 300 mA
            if(currents[max_index] > 0.3){
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
    } else {
        lastFrameInfo = getFrameInfo();
    }

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

    // controlling the stiffness of leg joints
    if (isRelaxing) {
        for( int i = naoth::JointData::RHipYawPitch; i <= naoth::JointData::LAnkleRoll; i++) {
            stiffnessController[i].updateFilter(getMotorJointData().position[i],getSensorJointData().position[i]);
            getMotorJointData().stiffness[i] = stiffnessController[i].control();
        }

        PLOT("StandMotion:AverageError:LKneePitch",stiffnessController[naoth::JointData::LKneePitch].getAverageError());
        PLOT("StandMotion:AverageError:RKneePitch",stiffnessController[naoth::JointData::RKneePitch].getAverageError());
        PLOT("StandMotion:Control:LKneePitch",stiffnessController[naoth::JointData::LKneePitch].control());
        PLOT("StandMotion:Control:RKneePitch",stiffnessController[naoth::JointData::RKneePitch].control());
    }
    
    if ( time >= totalTime && getMotionRequest().id != getId() ) {
      setCurrentState(motion::stopped);
      relaxedPoseInitialized  = false;

      // reset stuff for StandMotion:online_tuning
      jointOffsets.resetOffsets();
      for(int i = 0; i < naoth::JointData::numOfJoint; i++){
          jointMonitors[i].resetAll();
      }
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

  // used by StandMotion:stiffness_controller
  class StiffnessController{
  public:
      StiffnessController() {}

      void updateFilter(double motorData, double sensorData) {
          motorJointDataBuffer.add(motorData);

          if(motorJointDataBuffer.isFull()){
              motorToSensorError.add(sensorData - motorJointDataBuffer.first());
          }
      }

      double control() {
        if(motorToSensorError.isFull()){
            // linear function through points [0.08°,0.3] and [2.0°,1]
            double m = 0.7/1.92;
            double n = 1-m*2.0;

            double e = fabs(motorToSensorError.getAverage());

            if(e <= Math::fromDegrees(0.08)){
                return 0.3;
            } else if(e > Math::fromDegrees(2.0)) {
                return 1;
            } else {
                return m*Math::toDegrees(e) + n;
            }
        } else {
          return 0.7;
        }
      }

      double getAverageError() {
          return motorToSensorError.getAverage();
      }

      void resetFilter() {
          motorJointDataBuffer.clear();
          motorToSensorError.clear();
      }

  private:
      RingBuffer<double,4> motorJointDataBuffer;
      RingBufferWithSum<double,100> motorToSensorError;
  };

  StiffnessController stiffnessController[naoth::JointData::numOfJoint];

  // used by StandMotion:online_tuning
  FrameInfo lastFrameInfo;

  class JointMonitor
  {
  public:
      JointMonitor() {}

      void updateMonitor(double motorData, double sensorData, double cumCurrent) {
          motorJointDataBuffer.add(motorData);

          if(motorJointDataBuffer.isFull()){
              motorToSensorError.add(sensorData - motorJointDataBuffer.first());
          }

          current.add(cumCurrent);
      }

      void resetAll() {
          motorJointDataBuffer.clear();
          motorToSensorError.clear();
          current.clear();
      }

      void resetError() {
          motorToSensorError.clear();
      }

      double filteredError(){
          return motorToSensorError.getAverage();
      }

      double filteredCurrent(){
          return current.getAverage();
      }

  private:
      RingBuffer<double,4> motorJointDataBuffer;
      RingBufferWithSum<double,100> motorToSensorError;
      RingBufferWithSum<double,200> current;
  };

  JointMonitor jointMonitors[naoth::JointData::numOfJoint];

  class JointOffsets
  {
  public:
      JointOffsets():minimalStep(0.0013962634){}

      void resetOffsets(){
          for(int i = 0; i < naoth::JointData::numOfJoint; i++){
              offsets.position[i] = 0;
          }
      }

      void increaseOffset(int i){
          offsets.position[i] += minimalStep;
      }

      void decreaseOffset(int i){
          offsets.position[i] -= minimalStep;
      }

      double operator [](int i){
          return offsets.position[i];
      }

  private:
      JointData offsets;
      const double minimalStep; // [rad]
  };

  JointOffsets jointOffsets;
};

#endif  /* _StandMotion_H */

