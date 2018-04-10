/**
* @file NaoQiMotion.h
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

#include "NaoQiMotion.h"

using namespace naoth;

NaoQiMotion::NaoQiMotion()
  : 
  IKMotion(getInverseKinematicsMotionEngineService(), getMotionRequest().id, getMotionLock()),
    
  state(startingNaoQiMotion),
  lastState(stoppingNaoQiMotion),//HACK: need to be different from state (another enum value would require a default case in the switch statement)
  
  totalTime(0),
  time(0),
  height(-1000),
  standardStand(true),
  stiff_time(0),
  stiff_totalTime(50)
{
  // create 
  std::vector<double> x = {0.0, 0.5, 1.0};
  std::vector<double> y = {0.0, 0.5, 1.0};

  interpolationSpline.set_boundary(tk::spline::first_deriv,0.0, tk::spline::first_deriv,0.0, false);
  interpolationSpline.set_points(x,y);
}

void NaoQiMotion::execute()
{
  setCurrentState(motion::running);

  // HACK: turn off the hands
  //stiffness[JointData::LHand] = -1;
  //stiffness[JointData::RHand] = -1;

  PLOT("NaoQiMotion:State",(int)state);

  //double test_status = 2; //none
  //MODIFY("NaoQiMotion:BDRNaoQiStatus", test_status);
  //getBDRNaoQiStatus().behavior = static_cast<BDRNaoQiStatus::BDRNaoQiBehavior>(test_status);

  switch(state)
  {
  case startingNaoQiMotion:
  {
    if(lastState != state) {
      switch (getMotionRequest().id){
      case motion::bdr_entertainment:
          getBDRNaoQiRequest().behavior = BDRNaoQiRequest::entertainment;
          break;
      case motion::bdr_sit:
          getBDRNaoQiRequest().behavior = BDRNaoQiRequest::sit;
          break;
      default:
          //oops
          getBDRNaoQiRequest().behavior = BDRNaoQiRequest::none;
      }
      lastState = state;
    }

    if(getBDRNaoQiRequest().behavior == getBDRNaoQiStatus().behavior){
        state = runningNaoQiMotion;
    }
  }
  break;

  case runningNaoQiMotion:
  {
    if(lastState != state) {
      getBDRNaoQiRequest().disable_DCM_writings = true;
      lastState = state;
    }

    if(getMotionRequest().id != getId()){
        state = stoppingNaoQiMotion;
    }

    for (int i = 0; i < JointData::numOfJoint; i++)
    {
        getMotorJointData().position[i] = getSensorJointData().position[i];
    }
  }
  break;

  case stoppingNaoQiMotion:
  {
    if(lastState != state) {
      getBDRNaoQiRequest().behavior = BDRNaoQiRequest::none;
      lastState = state;
    }

    if(BDRNaoQiRequest::none == getBDRNaoQiStatus().behavior){
        state = goToStand;
    }

    for (int i = 0; i < JointData::numOfJoint; i++)
    {
        getMotorJointData().position[i] = getSensorJointData().position[i];
    }
  }
  break;

  case goToStand:
  {
    if(lastState != state) {
      stiff_time = 0;
      calcStandPose();
      lastState = state;
      getBDRNaoQiRequest().disable_DCM_writings = false;
     }

    if(interpolateToPose()){
        setCurrentState(motion::stopped);
    }

    if(stiff_time < stiff_totalTime){
        // interpolate form current stiffness into target stiffness
        for (int i = 0; i < JointData::numOfJoint; i++)
        {
            getMotorJointData().stiffness[i] = (1-stiff_time/stiff_totalTime) * getSensorJointData().stiffness[i] + (stiff_time/stiff_totalTime) * getMotorJointData().stiffness[i];
        }
    }
  }
  break;
  };
}//end execute

void NaoQiMotion::calcStandPose()
{
  // initialize standing
  standardStand = getMotionRequest().standardStand;
  height = getMotionRequest().standHeight;

  // init pose
  double comHeight = (height < 0.0) ? getEngine().getParameters().walk.hip.comHeight : getMotionRequest().standHeight;
  comHeight = Math::clamp(comHeight, 160.0, 270.0); // valid range
      
  // use the sensors to estimate the current pose
  startPose  = getEngine().getCoMFeetPoseBasedOnSensor();
  targetPose = getStandPose(comHeight, getEngine().getParameters().stand.hipOffsetX, getEngine().getParameters().stand.bodyPitchOffset, standardStand);

  targetPose.localInCoM();
  startPose.localInCoM();

  double speed = getEngine().getParameters().stand.speed;
  double distLeft = (targetPose.feet.left.translation - startPose.feet.left.translation).abs();
  double distRight = (targetPose.feet.right.translation - startPose.feet.right.translation).abs();
  double distMax = std::max(distLeft, distRight);
  totalTime = distMax / speed;
  time = 0;
} // end calcStandPose

bool NaoQiMotion::interpolateToPose()
{
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

void NaoQiMotion::applyPose(const InverseKinematic::CoMFeetPose& p)
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
