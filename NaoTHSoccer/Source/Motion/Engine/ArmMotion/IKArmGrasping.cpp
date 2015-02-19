/**
 * @file:  IKArmGrasping.cpp
 * @author: peppone
 * @author: <a href="mailto:stadie@informatik.hu-berlin.de">Oliver Stadie</a>
 * @author: <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 *
 * First Created on 5 luglio 2010, 18.01
 */

#include "IKArmGrasping.h"

#include <time.h> // for meassurement of elapsed times

using namespace std;

const double IKArmGrasping::maxHandDistance = 100.0;
const Vector3d IKArmGrasping::defaultGraspingCenter(145, 0, 20);

IKArmGrasping::IKArmGrasping()
  :
  //_pure_reachability_grid("reachibility_grid_arms_fixed_elbow_yaw.cfg"),
  //theReachibilityGridRight(_pure_reachability_grid),
  //theReachibilityGridLeft(_pure_reachability_grid),
  initialTime(1000),
  currentState(motion::stopped),
  graspingCenter(defaultGraspingCenter),
  ratio(100.0)
{
  //theReachibilityGridLeft.setRightFoot(true);

  DEBUG_REQUEST_REGISTER("IKArmGrasping:3D_debug_drwings", "...", false);
  DEBUG_REQUEST_REGISTER("IKArmGrasping:3D_draw_request", "...", false);
  DEBUG_REQUEST_REGISTER("IKArmGrasping:3D_draw_reachable_space", "...", false);

  DEBUG_REQUEST_REGISTER("IKArmGrasping:use_grasping_center_request", "...", false);
}

IKArmGrasping::~IKArmGrasping() 
{
}

void IKArmGrasping::execute() 
{
  
  /*
  // TEST: calculate the position of the hands
  Pose3D leftHand = getKinematicChainMotor().theLinks[KinematicChain::LForeArm].M;
  Pose3D rightHand = getKinematicChainMotor().theLinks[KinematicChain::RForeArm].M;

  Vector3d lHandOffset(NaoInfo::LowerArmLength+NaoInfo::HandOffsetX,0,0);
  Vector3d rHandOffset(NaoInfo::LowerArmLength+NaoInfo::HandOffsetX,0,0);
  leftHand.translate(lHandOffset);
  rightHand.translate(rHandOffset);
  */

  if (getMotionRequest().graspRequest.graspingState == GraspRequest::none) 
  {
    currentState = motion::stopped;
    return;
  }
  else
  {
    // init
    if(currentState == motion::stopped)
    {
      // default values
      ratio = 100.0;
      initialTime = 1000;
      graspingCenter = defaultGraspingCenter;

      theCurrentPose.chest = getKinematicChainMotor().theLinks[KinematicChain::Torso].M;
      theCurrentPose.arms.left = getKinematicChainMotor().theLinks[KinematicChain::LForeArm].M;
      theCurrentPose.arms.right = getKinematicChainMotor().theLinks[KinematicChain::RForeArm].M;

      Vector3d lHandOffset(NaoInfo::LowerArmLength+NaoInfo::HandOffsetX,0,0);
      Vector3d rHandOffset(NaoInfo::LowerArmLength+NaoInfo::HandOffsetX,0,0);
      theCurrentPose.arms.left.translate(lHandOffset);
      theCurrentPose.arms.right.translate(rHandOffset);
      theCurrentPose.localInChest();

      initialPose = theCurrentPose;
      hug(initialPose, graspingCenter, ratio);
    }
  }

  if(initialTime == 0)
  {
    calculateTrajectory(getMotionRequest());
  }
  else // initialize the erms before any grasping
  {
    if(initialTime > (int)getRobotInfo().basicTimeStep)
    {
      double t = ((double)getRobotInfo().basicTimeStep)/((double)initialTime);
      initialTime -= getRobotInfo().basicTimeStep;
      theCurrentPose = getEngine().interpolateArms(theCurrentPose, initialPose, t);
    }
    else
    {
      theCurrentPose = initialPose;
      initialTime = 0;
    }
  }

  // apply only the arms inverse kinematics
  getEngine().solveHandsIK(
    theCurrentPose.chest,
    theCurrentPose.arms.left,
    theCurrentPose.arms.right,
    getMotorJointData().position);
  
  //getMotorJointData().position[JointData::LWristYaw] = -Math::pi_2;
  //getMotorJointData().position[JointData::RWristYaw] = Math::pi_2;

  DEBUG_REQUEST("IKArmGrasping:3D_debug_drwings",
    debugDraw3D();
  );

  currentState = motion::running;
}//end execute




void IKArmGrasping::calculateTrajectory(const MotionRequest& motionRequest) 
{
  //TODO: find a better place for it
  static const Vector3d lHandOffset(NaoInfo::LowerArmLength+NaoInfo::HandOffsetX,0,0);
  static const Vector3d rHandOffset(NaoInfo::LowerArmLength+NaoInfo::HandOffsetX,0,0);

  // some parameters
  double minDistance = 30.0;
  MODIFY("IKGrasping:minDistance", minDistance);
  double maxDistance = 100.0;
  MODIFY("IKGrasping:maxDistance", maxDistance);
  double maxSpeed = 0.05; // mm/ms ~ m/s
  MODIFY("IKArmGrasping:maxSpeed", maxSpeed);
  double default_arm_stiffness = 0.5;
  MODIFY("IKArmGrasping:default_arm_stiffness", default_arm_stiffness);
  

  static Vector3d oldGraspingCenter = graspingCenter;
  // store the old graspingCenter 
  oldGraspingCenter = graspingCenter;

  // default grasping center
  graspingCenter = defaultGraspingCenter;

  // accept the grasping center request
  DEBUG_REQUEST("IKArmGrasping:use_grasping_center_request",
    const Pose3D& chestPose = getKinematicChainMotor().theLinks[KinematicChain::Torso].M;
    graspingCenter = chestPose.invert() * motionRequest.graspRequest.graspingPoint;
  );

  // limit the motion speed of the grasping center
  Vector3d graspingCenterMotionDelta = graspingCenter - oldGraspingCenter;
  if (graspingCenterMotionDelta.abs() > maxSpeed) {
    graspingCenter = oldGraspingCenter + graspingCenterMotionDelta.normalize(maxSpeed);
  }

  // draw the current grasping request
  DEBUG_REQUEST("IKArmGrasping:3D_draw_request",
    SPHERE("FF0000", 20, motionRequest.graspRequest.graspingPoint);
  );

  DEBUG_REQUEST("IKArmGrasping:3D_draw_reachable_space",
    drawReachabilityGrid();
  );

  //PLOT("IKArmGrasping:trajectory_size", trajectory.size());


  // estimate the current caused by the weight of the object
  double lShoulderPitchCurr = getSensorJointData().electricCurrent[JointData::LShoulderPitch];
  double rShoulderPitchCurr = getSensorJointData().electricCurrent[JointData::RShoulderPitch];
  double measured_weight_current = max(lShoulderPitchCurr, rShoulderPitchCurr);
  PLOT("IKArmGrasping:weight_current",measured_weight_current);
  

  // Determine if the grasping request changed
  static GraspRequest::GraspingState lastState = motionRequest.graspRequest.graspingState;
  bool newRequest = (lastState != motionRequest.graspRequest.graspingState);
  lastState = motionRequest.graspRequest.graspingState;
  PLOT("IKGrasping:newRequest", newRequest);

  // initial position
  InverseKinematic::ChestArmsPose p = initialPose;


  // control the distance
  switch (motionRequest.graspRequest.graspDistState) 
  {
    case GraspRequest::min_dist:
    {
      double step = 0.5;
      ratio = min(maxHandDistance, ratio - step);
      break;
    }
    case GraspRequest::no_sensor_dist:
    {
      double step = 3.0;
      MODIFY("IKGrasping:no_sensor_dist_step", step);
      ratio -= step;
      break;
    }
    case GraspRequest::thresh_dist_dist:
    {
      // determine controller parameters
      const Vector3d lHandPoint = getKinematicChainMotor().theLinks[KinematicChain::LForeArm].M * lHandOffset;
      const Vector3d rHandPoint = getKinematicChainMotor().theLinks[KinematicChain::RForeArm].M * rHandOffset;
      double measured_ratio = 0.5 * (lHandPoint - rHandPoint).abs();
      double step = 3.0;
      MODIFY("IKGrasping:thresh_dist_dist_step", step);

      PLOT("IKGrasping:thresh_dist_dist:measured_ratio", measured_ratio);
        
      // apply the controller
      ratio = min(ratio, measured_ratio - step);
      break;
    }
    case GraspRequest::thresh_curr_dist:
    {
      // determine controller parameters
      double lShoulderCurr = getSensorJointData().electricCurrent[JointData::LShoulderRoll];
      double rShoulderCurr = getSensorJointData().electricCurrent[JointData::RShoulderRoll];
      double measured_current = max(lShoulderCurr, rShoulderCurr);
      double step = 0.5;
      MODIFY("IKGrasping:thresh_curr_dist:step", step);
      double currFactor = 1.5;
      MODIFY("IKArmGrasping:thresh_curr_dist:factor", currFactor);
      double currThres = max(0.15, measured_weight_current*currFactor);
      MODIFY("IKGrasping:thresh_curr_dist:currThres", currThres);

      PLOT("IKGrasping:thresh_curr_dist:measured_current", measured_current);

      // apply controller
      //ratio = thresholdController(measured_current, currThres, step, newRequest);
      //ratio = test_thresholdController.update(ratio, measured_current, currThres, step);
      ratio -= (measured_current < currThres) * step;
      break;
    }
    case GraspRequest::thresh_force_dist:
    {
      break;
    }
    default: 
    {
      // don't do anything by default
      break;
    }
  }//end control the distance



  // control the stiffness
  switch (motionRequest.graspRequest.graspStiffState) 
  {
    case GraspRequest::max_stiff:
    {
      setArmStiffness(default_arm_stiffness);
      break;
    }
    case GraspRequest::integ_curr_stiff:
    {
      //integ_curr_stiff(newRequest);
      double stiffFactor = 2.0;
      MODIFY("IKArmGrasping:dynamic_stiff_current:factor", stiffFactor);
      for (int i = JointData::RShoulderRoll; i < JointData::RHipYawPitch; i++)
      {
        double jointCurr = getSensorJointData().electricCurrent[i];
        double value = jointCurr*stiffFactor;
        double stiffness = Math::clamp(value,0.3,1.0);
        getMotorJointData().stiffness[i] = stiffness;
      }
      break;
    }
    case GraspRequest::integ_force_stiff:
    {
      //integ_force_stiff(newRequest);
      double stiffFactor = 15.0;
      MODIFY("IKArmGrasping:dynamic_stiff_angle:factor", stiffFactor);
      for (int i = JointData::RShoulderRoll; i < JointData::RHipYawPitch; i++)
      {
        double measuredAngle = getSensorJointData().position[i];
        double targetAngle = getMotorJointData().position[i];
        double value = fabs(measuredAngle - targetAngle)*stiffFactor;
        double stiffness = Math::clamp(value,-1.0,1.0);
        getMotorJointData().stiffness[i] = stiffness;
      }
      break;
    }
    case GraspRequest::p_force_stiff:
    {
      break;
    }
    default: {
      // don't do anything by default
      break;
    }
  }//end control the stiffness

   
  switch (motionRequest.graspRequest.graspingState) 
  {
    case GraspRequest::empty:
    {
      // do nothing
      break;
    }

    case GraspRequest::open:
    {
      double step = 0.5;
      ratio = min(maxHandDistance, ratio + step);
      setArmStiffness(0.5);
      break;
    }
    
    case GraspRequest::no_dist:
    {
      // determine controller parameters
      const Vector3d lHandPoint = getKinematicChainMotor().theLinks[KinematicChain::LForeArm].M * lHandOffset;
      const Vector3d rHandPoint = getKinematicChainMotor().theLinks[KinematicChain::RForeArm].M * rHandOffset;
      double measured_ratio = 0.5 * (lHandPoint - rHandPoint).abs();
      double step = 5.0;
      MODIFY("IKGrasping:thresh_dist_dist:step", step);

      // apply the controller
      ratio = measured_ratio - step;

      // apply a minimum to the hands distance
      double maxDistance = 80.0;
      MODIFY("IKGrasping:thresh_dist_dist:minDistance", minDistance);
      MODIFY("IKGrasping:thresh_dist_dist:maxDistance", maxDistance);
      ratio = Math::clamp(ratio, minDistance, maxDistance);

      // set the hardness
      double minStiffness = 0.3;
      MODIFY("IKGrasping:thresh_dist_dist:minStiffness", minStiffness);
      setArmStiffness(minStiffness);

      break;
    }
    case GraspRequest::stiff_eq_pressure:
    {
      return;
    }

    case GraspRequest::test_motion_model:
    {
      break;
    }

    case GraspRequest::test_roundtrip_delay:
    {
      test_roundtrip_delay();
      return;
    }

    default: break;
  }//end switch


  // apply a minimum to the hands distance
  ratio = max(minDistance, ratio);
  PLOT("IKGrasping:thresh_force_dist_ratio", ratio);


  // some debug
  MODIFY("IKGrasping:graspingCenter:x", graspingCenter.x);
  MODIFY("IKGrasping:graspingCenter:y", graspingCenter.y);
  MODIFY("IKGrasping:graspingCenter:z", graspingCenter.z);
  MODIFY("IKGrasping:open_ratio", ratio);

  // calculate the hands positions
  hug(p, graspingCenter, ratio);
  

  // check if reachable
  //Vector3<int> leftHandPointGrid = theReachibilityGridLeft.pointToGridCoordinates(p.arms.left.translation);
  //Vector3<int> rightHandPointGrid = theReachibilityGridRight.pointToGridCoordinates(p.arms.right.translation);

  // HACK: check whethet this works properly
  /*
  if( theReachibilityGridRight.gridPointReachable(rightHandPointGrid) &&
      theReachibilityGridLeft.gridPointReachable(leftHandPointGrid) &&
      fabs(theMotorJointData.position[JointData::LElbowRoll]) > 0.5 &&
      fabs(theMotorJointData.position[JointData::RElbowRoll]) > 0.5)
      */
  {
    // TODO: interpolate?
    theCurrentPose = p;

    InverseKinematic::ChestArmsPose q;
    q.chest = getKinematicChainMotor().theLinks[KinematicChain::Torso].M;
    q.arms.left = getKinematicChainMotor().theLinks[KinematicChain::LForeArm].M;
    q.arms.right = getKinematicChainMotor().theLinks[KinematicChain::RForeArm].M;

    q.arms.left.translate(lHandOffset);
    q.arms.right.translate(rHandOffset);
    q.localInChest();
  }
}//end calculateTrajectory



void IKArmGrasping::setShoulderPitchStiffness(double value)
{
  double stiffness = Math::clamp(value,-1.0,1.0);

  getMotorJointData().stiffness[MotorJointData::LShoulderPitch] = stiffness;
  getMotorJointData().stiffness[MotorJointData::LElbowYaw] = stiffness;

  getMotorJointData().stiffness[MotorJointData::RShoulderPitch] = stiffness;
  getMotorJointData().stiffness[MotorJointData::RElbowYaw] = stiffness;
}//end setShoulderPitchStiffness

void IKArmGrasping::setArmStiffness(double value)
{
  double stiffness = Math::clamp(value,-1.0,1.0);

  getMotorJointData().stiffness[MotorJointData::LElbowYaw] = stiffness;
  getMotorJointData().stiffness[MotorJointData::LElbowRoll] = stiffness;
  getMotorJointData().stiffness[MotorJointData::LShoulderPitch] = stiffness;
  getMotorJointData().stiffness[MotorJointData::LShoulderRoll] = stiffness;

  getMotorJointData().stiffness[MotorJointData::RElbowYaw] = stiffness;
  getMotorJointData().stiffness[MotorJointData::RElbowRoll] = stiffness;
  getMotorJointData().stiffness[MotorJointData::RShoulderPitch] = stiffness;
  getMotorJointData().stiffness[MotorJointData::RShoulderRoll] = stiffness;
}//end setArmStiffness


void IKArmGrasping::hug(InverseKinematic::ChestArmsPose& p, const Vector3d& graspingCenter, double ratio) 
{
  p.localInChest();

  Vector3d leftDir(0, 1, 0);
  p.arms.left.translation = graspingCenter + leftDir.normalize(ratio);

  Vector3d rightDir(0, -1, 0);
  p.arms.right.translation = graspingCenter + rightDir.normalize(ratio);
}//end hug


/** Output(t) = Output(t-1) + (Input(t) < Threshold) * Step */
double IKArmGrasping::thresholdController(double input, double threshold, double step, bool reset) 
{
  double output = 0;
  static double prevOutput = 0;
  if (reset) 
  {
    prevOutput = 0;
  }
  output = prevOutput + (input < threshold) * step;
  prevOutput = output;
  return output;
} // end thresholdController

/**
 * Output(t) = Integral{weight * (inputDesired(t-1) - input(t-1))}
 */
double IKArmGrasping::integralController(double input, double inputDesired, double weight, bool reset) 
{
  static double output = 0;
  if (reset) 
  {
    output = 0;
  }
  output += weight * (inputDesired - input);
  return output;
} // end integralController


void IKArmGrasping::test_roundtrip_delay() 
{
  // calculate the time since first applying this debug request
  static unsigned int timeOnFirstRequest = getFrameInfo().getTime();
  double timeSinceFirstRequest = static_cast<double>(getFrameInfo().getTimeSince(timeOnFirstRequest));
  PLOT("IKGrasping:test_roundtrip_delay_timeSinceFirstRequest", timeSinceFirstRequest);

  // remove stiffness
  setArmStiffness(-1.0);

  double alpha_messured = getSensorJointData().position[MotorJointData::LShoulderPitch];
  double alpha_ordered = 0;

  if (sin(static_cast<double>(timeSinceFirstRequest) / 1000.0 / 4.0) > 0)
  {
    alpha_ordered = Math::pi / 2 * sin(static_cast<double>(timeSinceFirstRequest) / 1000.0 / 2.0);
  }

  PLOT("IKGrasping:test_roundtrip_delay_alpha_messured", alpha_messured);
  PLOT("IKGrasping:test_roundtrip_delay_alpha_ordered", alpha_ordered);

  getMotorJointData().position[MotorJointData::LShoulderPitch] = alpha_ordered;
}//end test_roundtrip_delay

/**
 * DEBUG-Request-Methods
 */


void IKArmGrasping::debugDraw3D() 
{
  // HACK: static access to the black board
  //const Kinematics::Link* lio = MotionBlackBoard::getInstance().theKinematicChain.theLinks;

  // draw the current grasping center
  const Pose3D& chestPose = getKinematicChainMotor().theLinks[KinematicChain::Torso].M;
  SPHERE("CD7F32", 20, chestPose * graspingCenter);

  // visualization of the current requested pose
  InverseKinematic::ChestArmsPose currentPose = theCurrentPose;
  currentPose.localInChest();

  // draw the right hand request
  Pose3D globalHandRight = chestPose * currentPose.arms.right;
  Vector3d xAxis(60, 0, 0);
  Vector3d yAxis(0, 60, 0);
  Vector3d zAxis(0, 0, 60);

  // transform into the hands coordinate system
  xAxis = globalHandRight * xAxis;
  yAxis = globalHandRight * yAxis;
  zAxis = globalHandRight * zAxis;

  BOX_3D("DC143C", 15, 15, 15, globalHandRight);
  LINE_3D(ColorClasses::red, globalHandRight.translation, xAxis);
  LINE_3D(ColorClasses::green, globalHandRight.translation, yAxis);
  LINE_3D(ColorClasses::blue, globalHandRight.translation, zAxis);


  // draw the right hand request
  Pose3D globalHandLeft = chestPose * currentPose.arms.left;
  // transform into the hands coordinate system
  xAxis = Vector3d(60, 0, 0);
  yAxis = Vector3d(0, 60, 0);
  zAxis = Vector3d(0, 0, 60);

  xAxis = globalHandLeft * xAxis;
  yAxis = globalHandLeft * yAxis;
  zAxis = globalHandLeft * zAxis;

  BOX_3D("FFA07A", 15, 15, 15, globalHandLeft);
  LINE_3D(ColorClasses::red, globalHandLeft.translation, xAxis);
  LINE_3D(ColorClasses::green, globalHandLeft.translation, yAxis);
  LINE_3D(ColorClasses::blue, globalHandLeft.translation, zAxis);
}//end debugDraw3D

void IKArmGrasping::drawReachabilityGrid() const 
{
  /*
  static int z = 0;
  const Pose3D& chestPose = theBlackBoard.theKinematicChain.theLinks[KinematicChain::Torso].M;
  const TransformedReachibilityGrid& reachibilityGrid = theReachibilityGridLeft;

  z = (z + 1) % reachibilityGrid.grid.resolution;

  Vector3<int> point;
  point.z = z;

  for (point.x = 0; point.x < reachibilityGrid.grid.resolution; point.x++) {
    for (point.y = 0; point.y < reachibilityGrid.grid.resolution; point.y++) {
      if (reachibilityGrid.gridPointReachable(point)) {
        Vector3d pointInGlobalCoords = reachibilityGrid.gridPointToWorldCoordinates(point);

        SPHERE("FF0000", 5, chestPose * pointInGlobalCoords);
      }//end if
    }//end for
  }//end for
  */
}//end drawReachabilityGrid
