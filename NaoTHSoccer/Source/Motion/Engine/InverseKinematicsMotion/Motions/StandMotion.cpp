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

#include <array>

using namespace naoth;

StandMotion::StandMotion()
  :
  IKMotion(getInverseKinematicsMotionEngineService(), motion::stand, getMotionLock()),

  firstRun(true),
  state(GotoStandPose),
  lastState(None),
  state_time(0),

  totalTime(0),
  time(0),
  
  height(-1000),
  standardStand(true),
  isLiftedUp(false),

  fullCorrection(true),
  stiffnessIsReady(false),
  lastFrameInfo(getFrameInfo())
{
  // create
  std::vector<double> x = {0.0, 0.5, 1.0};
  std::vector<double> y = {0.0, 0.5, 1.0};

  interpolationSpline.set_boundary(tk::spline::first_deriv,0.0, tk::spline::first_deriv,0.0, false);
  interpolationSpline.set_points(x,y);

  getMotionStatus().target_reached = false;
}


void StandMotion::execute()
{
  setCurrentState(motion::running);

  // HACK: make it parameter or so?
  const double timeBeforeRelax = static_cast<double>(getRobotInfo().basicTimeStep*100);
  const double stiffDelta = getRobotInfo().getBasicTimeStepInSecond();

  // update joint monitors
  for( int i = naoth::JointData::RShoulderRoll; i <= naoth::JointData::LAnkleRoll; i++) {
    // are the jointOffsets applied twice? Doesn't getMotorJointData contain the offsets already?
    jointMonitors[i].updateMonitor(getMotorJointData().position[i] + jointOffsets[i], getSensorJointData().position[i],getSensorJointData().electricCurrent[i]);
  }

  PLOT("StandMotion:State",(int)state);

  // a change of stand pose was induced from outside, either by lifting the robot 
  // or by changing the parameters of the stand
  if( firstRun ||
      isLiftedUp    != getBodyState().isLiftedUp ||
      height        != getMotionRequest().standHeight ||
      standardStand != getMotionRequest().standardStand) 
  {
    fullCorrection = true;
    
    // force new correction
    state = GotoStandPose;
    lastState = None;
  }

  // remember for later
  isLiftedUp = getBodyState().isLiftedUp;

  switch(state)
  {
  case GotoStandPose:
  {
    // initialize on fist execution
    if(lastState != state) 
    {
      lastState = state;

      // set target stiffness from parameters
      setStiffnessBuffer(getEngine().getParameters().stand.stiffnessGotoPose);

      calcStandPose(fullCorrection);

      stiffnessIsReady = false;
    }

    bool d = interpolateToPose();
    PLOT("StandMotion:stiffnessIsReady",stiffnessIsReady);
    PLOT("StandMotion:interpolateToPose",d);

    if(stiffnessIsReady && d) 
    {
      getMotionStatus().target_reached = true;
      if(getMotionRequest().id != getId())
      {
        // NOTE: this is the opnly place where the stand motion can exit
        setCurrentState(motion::stopped);
        getMotionStatus().target_reached = false;
      }
      // HACK: always wait some time before going to relax mode
      else if (static_cast<double>(state_time) > totalTime + timeBeforeRelax
                  && getEngine().getParameters().stand.relax.enable
                  && relaxedPoseIsStillOk() //only relax if a valid stand pose is reached
                  && !getMotionRequest().disable_relaxed_stand) // and requested
      {
        state = Relax;
      }
    }
  }
  break;

  case Relax:

    // initialize relax on fist execution
    if(lastState != state) 
    {
      lastState = state;

      // set target stiffness from parameters
      setStiffnessBuffer(getEngine().getParameters().stand.stiffnessRelax);

      // move to the pose that is measured by our sensors = relax :)
      startPose = targetPose;
      targetPose = getEngine().getCoMFeetPoseBasedOnSensor();
      time = 0;
      totalTime = 1000;

      // reset stuff for StandMotion:online_tuning
      jointOffsets.resetOffsets();
      for(int i = 0; i < naoth::JointData::numOfJoint; i++){
        jointMonitors[i].resetAll();
      }
    }

    // run adjustments after the relaxed pose had been reached
    if(interpolateToPose()) 
    {
      if(getEngine().getParameters().stand.relax.jointOffsetTuning.enable){
        tuneJointOffsets();
      }

      if(getEngine().getParameters().stand.relax.stiffnessControl.enable){
        tuneStiffness();
      }

      // correct the pose needs to be corrected (also before leaving stand)
      if ( getMotionRequest().id != getId() || 
           !relaxedPoseIsStillOk() ||
           getMotionRequest().disable_relaxed_stand) 
      {
        fullCorrection = false; // only adjust the com, not the feet
        state = GotoStandPose;
      } 
    }
    
    break;

  default:
    THROW("[StandMotion] unexpected (state, lastState) = (" << state << ", " << lastState);
  } // end switch

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

  firstRun = false;
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


void StandMotion::calcStandPose(bool fullCorrection)
{
  // use the sensors to estimate the current pose
  startPose  = getEngine().getCurrentCoMFeetPose();//getEngine().getCoMFeetPoseBasedOnModel();
  
  // HACK: don't do anything if after walk
  //       copy the pose only the first time
  if(firstRun && getMotionStatus().lastMotion == motion::walk) 
  {
    targetPose = startPose;
  }
  else // calculate full correction pose
  {
    // adjust the positions of the feet only if a full correction of the standing pose is requested
    standardStand = fullCorrection && getMotionRequest().standardStand;
    
    // init pose
    height = getMotionRequest().standHeight;
    double comHeight = (height < 0.0) ? getWalk2018Parameters().zmpPlanner2018Params.comHeight : getMotionRequest().standHeight;
    comHeight = Math::clamp(comHeight, 160.0, 270.0); // valid range

    targetPose = getStandPose(comHeight,
                            getEngine().getParameters().stand.hipOffsetX,
                            getEngine().getParameters().stand.bodyPitchOffset,
                            standardStand);
  }

  // TODO: there mus be a better way to do it
  // estimate the maximal error between the targetPose and startPose in order to estimate the time
  // which is necessary to move from one to another. In order to take the rotation into account we 
  // calculate the maximal error from the perspective of each of the poses: left/right foot and com.
  
  // error from the perspective of the left foot
  targetPose.localInLeftFoot();
  startPose.localInLeftFoot();
  const double distLeft = std::max(
    (targetPose.com.translation        - startPose.com.translation).abs(), 
    (targetPose.feet.right.translation - startPose.feet.right.translation).abs()
  );

  // error from the perspective of the right foot
  targetPose.localInRightFoot();
  startPose.localInRightFoot();
  const double distRight = std::max(
    (targetPose.com.translation        - startPose.com.translation).abs(), 
    (targetPose.feet.left.translation  - startPose.feet.left.translation).abs()
  );

  // error from the perspective of the com
  targetPose.localInCoM();
  startPose.localInCoM();
  const double distCom = std::max(
    (targetPose.feet.left.translation  - startPose.feet.left.translation).abs(), 
    (targetPose.feet.right.translation - startPose.feet.right.translation).abs()
  );
  
  double distMax = std::max(std::max(distLeft,distRight),distCom);

  // initialize the time
  double speed = getEngine().getParameters().stand.speed;
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

  if(getFrameInfo().getTime() > lastFrameInfo.getTime() + getEngine().getParameters().stand.relax.jointOffsetTuning.deadTime)
  {
    const std::array<JointData::JointID, 4> jointToMonitor = {
      JointData::LKneePitch, JointData::RKneePitch, JointData::LAnklePitch, JointData::RAnklePitch
    };

    PLOT("StandMotion:filteredCurrent:LKneePitch", jointMonitors[JointData::LKneePitch].filteredCurrent());
    PLOT("StandMotion:filteredCurrent:RKneePitch", jointMonitors[JointData::RKneePitch].filteredCurrent());
    PLOT("StandMotion:filteredCurrent:LAnklePitch",jointMonitors[JointData::LAnklePitch].filteredCurrent());
    PLOT("StandMotion:filteredCurrent:RAnklePitch",jointMonitors[JointData::RAnklePitch].filteredCurrent());

    // find the joint with the max current
    JointData::JointID maxId = jointToMonitor.front();
    for(JointData::JointID id: jointToMonitor) {
      if(jointMonitors[id].filteredCurrent() > jointMonitors[maxId].filteredCurrent()) {
        maxId = id;
      }
    }

    // DEBUG: print max joint
    //std::cout << "max: " << maxId << " : " << jointMonitors[maxId].filteredCurrent() << std::endl; 

    // if greater than Threshold then try to tune the offsets
    if(jointMonitors[maxId].filteredCurrent() > getEngine().getParameters().stand.relax.jointOffsetTuning.currentThreshold) {
      // HACK: make ot more general (maybe adjust in the direction of the error)
      if(maxId == JointData::LKneePitch || maxId == JointData::RKneePitch) {
        jointOffsets.increaseOffset(maxId);
      } else {
        jointOffsets.decreaseOffset(maxId);
      }
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


