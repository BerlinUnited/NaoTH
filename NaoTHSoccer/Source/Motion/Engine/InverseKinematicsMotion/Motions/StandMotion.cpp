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

#include "StandMotion.h"

using namespace naoth;

StandMotion::StandMotion()
  :
  IKMotion(getInverseKinematicsMotionEngineService(), motion::stand, getMotionLock()),

  state(GotoStandPose),
  lastState(Relax),//HACK: need to be different from state (another enum value would require a default case in the switch statement)
  state_time(0),

  totalTime(0),
  time(0),
  height(-1000),
  standardStand(true),
  stiffnessIsReady(false),
  relaxedPoseInitialized(false),
  resetedAfterLifting(false),
  lastFrameInfo(getFrameInfo())
{
  // create
  std::vector<double> x = {0.0, 0.5, 1.0};
  std::vector<double> y = {0.0, 0.5, 1.0};

  interpolationSpline.set_boundary(tk::spline::first_deriv,0.0, tk::spline::first_deriv,0.0, false);
  interpolationSpline.set_points(x,y);
}


void StandMotion::execute()
{
  // HACK: make it parameter or so?
  const double timeBeforeRelax = static_cast<double>(getRobotInfo().basicTimeStep*100);

  setCurrentState(motion::running);
  double stiffDelta = getRobotInfo().getBasicTimeStepInSecond();

  // update joint monitors
  for( int i = naoth::JointData::RShoulderRoll; i <= naoth::JointData::LAnkleRoll; i++) {
    // are the jointOffsets applied twice? Doesn't getMotorJointData contain the offsets already?
    jointMonitors[i].updateMonitor(getMotorJointData().position[i] + jointOffsets[i], getSensorJointData().position[i],getSensorJointData().electricCurrent[i]);
  }

  PLOT("StandMotion:State",(int)state);

  switch(state)
  {
  case GotoStandPose:
  {
    // set stiffness from parameters
    setStiffnessBuffer(getEngine().getParameters().stand.stiffnessGotoPose);

    if(lastState != state) {
      calcStandPose();
      lastState = state;
      stiffnessIsReady = false;
    }

    bool d = interpolateToPose();
    PLOT("StandMotion:stiffnessIsReady",stiffnessIsReady);
    PLOT("StandMotion:interpolateToPose",d);

    if(stiffnessIsReady && d) {
      if(getMotionRequest().id != getId())  {
        setCurrentState(motion::stopped);

        // HACK: wait some time before going to relax mode
      } else if (static_cast<double>(state_time) > totalTime + timeBeforeRelax
                  && !getBodyState().isLiftedUp
                  && getEngine().getParameters().stand.relax.enable
                  && relaxedPoseIsStillOk() //only relax if a valid stand pose is reached
                  && !getMotionRequest().disable_relaxed_stand) { // and requested
        state = Relax;
      }

      //applyPose(targetPose);
    }
  }
  break;

  case Relax:
    setStiffnessBuffer(getEngine().getParameters().stand.stiffnessRelax);

    // initialize relax on fist execution
    if(lastState != state) {

      startPose = targetPose;//getEngine().getCurrentCoMFeetPose();
      targetPose = getEngine().getCoMFeetPoseBasedOnSensor();
      time = 0;
      totalTime = 1000;

      //relaxedPose = getEngine().getHipFeetPoseBasedOnSensor();
      lastState = state;

      // reset stuff for StandMotion:online_tuning
      jointOffsets.resetOffsets();
      for(int i = 0; i < naoth::JointData::numOfJoint; i++){
          jointMonitors[i].resetAll();
      }
    }

    if(interpolateToPose()) {

      // NOTE: we do not correct the pose before leaving stand
      if ( getMotionRequest().id != getId()) {
        //setCurrentState(motion::stopped);
        state = GotoStandPose;
      } // the pose needs to be corrected
      else if ( getBodyState().isLiftedUp
                  || height != getMotionRequest().standHeight // requested height changed
                  || standardStand != getMotionRequest().standardStand
                  || !relaxedPoseIsStillOk())
      {
        state = GotoStandPose;
      }

      //applyPose(relaxedPose);

      if(getEngine().getParameters().stand.relax.jointOffsetTuning.enable){
        tuneJointOffsets();
      }

      if(getEngine().getParameters().stand.relax.stiffnessControl.enable){
        tuneStiffness();
      }
    }
    break;
  };

  // update the time of the current step
  if(lastState != state) {
    state_time = 0;
  } else {
    state_time += getRobotInfo().basicTimeStep;
  }

  // apply stiffness from buffer
  /*
  for( int i = naoth::JointData::RShoulderRoll; i < naoth::JointData::numOfJoint; i++) {
    getMotorJointData().stiffness[i] = stiffness[i];
  }*/
  stiffnessIsReady = setStiffness(getMotorJointData(), getSensorJointData(), stiffness, stiffDelta, naoth::JointData::RHipYawPitch, naoth::JointData::LWristYaw);

  //turnOffStiffnessWhenJointIsOutOfRange();
}//end execute

void StandMotion::setStiffnessBuffer(double s)
{

  for( int i = naoth::JointData::RShoulderRoll; i<naoth::JointData::numOfJoint; i++) {
    stiffness[i] = s;
  }
  // HACK: turn off the hands
  stiffness[JointData::LHand] = -1;
  stiffness[JointData::RHand] = -1;
}


void StandMotion::calcStandPose()
{
  // initialize standing
  standardStand = getMotionRequest().standardStand;
  height = getMotionRequest().standHeight;

  // init pose
  double comHeight = (height < 0.0) ? getWalk2018Parameters().zmpPlanner2018Params.comHeight : getMotionRequest().standHeight;
  comHeight = Math::clamp(comHeight, 160.0, 270.0); // valid range

  // use the sensors to estimate the current pose
  startPose  = getEngine().getCurrentCoMFeetPose();//getEngine().getCoMFeetPoseBasedOnModel();//
  targetPose = getStandPose(comHeight,
                            getEngine().getParameters().stand.hipOffsetX,
                            getEngine().getParameters().stand.bodyPitchOffset,
                            standardStand);

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
} // end calcStandPose


bool StandMotion::interpolateToPose()
{
  PLOT("StandMotion:totalTime", totalTime);
  PLOT("StandMotion:time", time);
  // execute the stand motion
  if(totalTime >= 0 && time <= totalTime)
  {
      InverseKinematic::CoMFeetPose p;

      if(totalTime > 0) {
          double t = time/totalTime;
          //double k = Math::clamp(0.5*(1.0-cos(t*Math::pi)), 0.0, 1.0);
          double k = interpolationSpline(t);
          p = getEngine().interpolate(startPose, targetPose, k);
      } else {
          p = targetPose;
      }

      time += getRobotInfo().basicTimeStep;

      applyPose(p);
      return false;
  }

  // done
  applyPose(targetPose);
  return true;
}//end interpolateToPose


void StandMotion::applyPose(const InverseKinematic::CoMFeetPose& p)
{
  bool solved = false;
  InverseKinematic::HipFeetPose c = getEngine().controlCenterOfMass(getMotorJointData(), p, solved, false);

  //InverseKinematic::HipFeetPose c(tc);
  if(getCalibrationData().calibrated)
  {
    c.localInLeftFoot();

    if(getEngine().getParameters().stand.enableStabilization) {
      getEngine().rotationStabilize(
        getInertialModel(),
        getGyrometerData(),
        getRobotInfo().getBasicTimeStepInSecond(),
        getEngine().getParameters().stand.stabilization.rotation.P,
        getEngine().getParameters().stand.stabilization.rotation.VelocityP,
        getEngine().getParameters().stand.stabilization.rotation.D,
        c);
    } else if(getEngine().getParameters().stand.enableStabilizationRC16) {
      getEngine().rotationStabilizeRC16(
        getInertialSensorData().data,
        getGyrometerData(),
        getRobotInfo().getBasicTimeStepInSecond(),
        getEngine().getParameters().stand.stabilization.rotationRC16.P,
        getEngine().getParameters().stand.stabilization.rotationRC16.VelocityP,
        getEngine().getParameters().stand.stabilization.rotationRC16.D,
        c);
    }
  }

  getEngine().solveHipFeetIK(c);
  getEngine().copyLegJoints(getMotorJointData().position);
}//end applyPose


void StandMotion::tuneJointOffsets()
{
  jointOffsets.setMinimalStep(getEngine().getParameters().stand.relax.jointOffsetTuning.minimalJointStep);

  if(getFrameInfo().getTime() - lastFrameInfo.getTime() > getEngine().getParameters().stand.relax.jointOffsetTuning.deadTime)
  {
    std::map<JointData::JointID, double> currents = {
      {JointData::LKneePitch,  jointMonitors[JointData::LKneePitch].filteredCurrent()},
      {JointData::RKneePitch,  jointMonitors[JointData::RKneePitch].filteredCurrent()},
      {JointData::LAnklePitch, jointMonitors[JointData::LAnklePitch].filteredCurrent()},
      {JointData::RAnklePitch, jointMonitors[JointData::RAnklePitch].filteredCurrent()}
    };

    PLOT("StandMotion:filteredCurrent:LKneePitch", currents[JointData::LKneePitch]);
    PLOT("StandMotion:filteredCurrent:RKneePitch", currents[JointData::RKneePitch]);
    PLOT("StandMotion:filteredCurrent:LAnklePitch",currents[JointData::LAnklePitch]);
    PLOT("StandMotion:filteredCurrent:RAnklePitch",currents[JointData::RAnklePitch]);

    // try the C++ 11 way of findig the maximal index :)
    auto max_joint = std::max_element(currents.begin(), currents.end());

    // if greater than Threshold then try to tune the offsets
    if(max_joint != currents.end() && max_joint->second > getEngine().getParameters().stand.relax.jointOffsetTuning.currentThreshold) {
      jointOffsets.decreaseOffset(max_joint->first);
    }

    lastFrameInfo = getFrameInfo();
  }

  for( int i = naoth::JointData::RHipYawPitch; i <= naoth::JointData::LAnkleRoll; i++) {
    getMotorJointData().position[i] += jointOffsets[i];
  }
}//end tuneJointOffsets

void StandMotion::tuneStiffness()
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
}// end tuneStiffness


bool StandMotion::relaxedPoseIsStillOk()
{
  InverseKinematic::CoMFeetPose currentPose = getEngine().getCurrentCoMFeetPose();
  currentPose.localInLeftFoot();

  // NOTE: calculated while going to the stand pose
  InverseKinematic::CoMFeetPose target(targetPose);
  target.localInLeftFoot();

  PLOT("StandMotion:Deviations:Distance", (currentPose.com.translation - target.com.translation).abs());
  PLOT("StandMotion:Deviations:RotationX", Math::toDegrees(fabs(currentPose.com.rotation.getXAngle() - target.com.rotation.getXAngle())));
  PLOT("StandMotion:Deviations:RotationY", Math::toDegrees(fabs(currentPose.com.rotation.getYAngle() - target.com.rotation.getYAngle())));

  double max_rotation_error = getEngine().getParameters().stand.relax.allowedRotationDeviation;
  double max_translation_error = getEngine().getParameters().stand.relax.allowedDeviation;

  return   (currentPose.com.translation - target.com.translation).abs() < max_translation_error
    && fabs(currentPose.com.rotation.getXAngle() - target.com.rotation.getXAngle()) < max_rotation_error
    && fabs(currentPose.com.rotation.getYAngle() - target.com.rotation.getYAngle()) < max_rotation_error;
}// end relaxedPoseIsStillOk

/*
void StandMotion::turnOffStiffnessWhenJointIsOutOfRange()
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
*/


