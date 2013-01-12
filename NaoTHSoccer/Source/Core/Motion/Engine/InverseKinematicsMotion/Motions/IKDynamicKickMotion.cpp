/**
* @file IKDynamicKickMotion.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class IKDynamicKickMotion
*/

#include "IKDynamicKickMotion.h"

// Tools

// Debug
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings3D.h"

//#include "Motion/Motion.h"


ReachibilityGrid IKDynamicKickMotion::basicReachibilityGrid;

IKDynamicKickMotion::IKDynamicKickMotion()
  :
  IKMotion(getInverseKinematicsMotionEngineService(), motion::kick, getMotionLock()),
  reachibilityGrid(basicReachibilityGrid),
  theParameters(getEngine().getParameters().kick),
  numberOfPreKickSteps(0),
  theKickingFoot(KickRequest::right), // right foot by default
  kickState(kick_prepare),
  oldKickState(kickState),
  action_done(false),
  state_start_time(0),
  state_frame_count(0)
{

  wrap_up_made = false;

  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    getMotorJointData().stiffness[i] = 1.0;
  }

  DEBUG_REQUEST_REGISTER("IKDynamicKickMotion:reachability_grid_2d","draw the reachable points in the field viewer",false);
  DEBUG_REQUEST_REGISTER("IKDynamicKickMotion:reachability_grid_3d","draw the reachable points in the 3D viewer",false);
  DEBUG_REQUEST_REGISTER("IKDynamicKickMotion:load_point","draw the load point",false);
  DEBUG_REQUEST_REGISTER("IKDynamicKickMotion:retraction_trajectory","plot the retraction trajectory in 3D viewer",false);

  DEBUG_REQUEST_REGISTER("IKDynamicKickMotion:kick_point", "...",false);

  DEBUG_REQUEST_REGISTER("IKDynamicKickMotion:kick_request","kick point in foot koordinates",false);
  DEBUG_REQUEST_REGISTER("IKDynamicKickMotion:kick_request_foot_coord","kick point in foot koordinates",false);
  DEBUG_REQUEST_REGISTER("IKDynamicKickMotion:hiting_point","kick point in foot koordinates",false);
  DEBUG_REQUEST_REGISTER("IKDynamicKickMotion:hiting_point_grid","kick point in foot koordinates",false);

  //DEBUG_REQUEST_REGISTER("IKDynamicKickMotion:reachability_grid_2d","draw the reachable points in the field viewer",false);
  //DEBUG_REQUEST_REGISTER("IKDynamicKickMotion:reachability_grid_3d","draw the reachable points in the 3D viewer",false);
  
  DEBUG_REQUEST_REGISTER("IKDynamicKickMotion:retraction_point","draw the load point",false);
  //DEBUG_REQUEST_REGISTER("IKDynamicKickMotion:retraction_trajectory","plot the retraction trajectory in 3D viewer",false);

  DEBUG_REQUEST_REGISTER("IKDynamicKickMotion:kick_request_geometry","...",false);
  DEBUG_REQUEST_REGISTER("IKDynamicKickMotion:kick_request_in_grid","...",false);
}

IKDynamicKickMotion::~IKDynamicKickMotion()
{
}

void IKDynamicKickMotion::execute()
{

  //
  theKickingFoot = getMotionRequest().kickRequest.kickFoot;


  // first time
  if (isStopped())
  {
    currentPose.pose = getEngine().getCurrentCoMFeetPose();
    lastCoMPose = currentPose.pose;
    localInStandFoot(currentPose);
  }


  //
  calculateTrajectory(getMotionRequest());


  if(!trajectory.empty())
  {
    // take the next pose to be executed
    KickPose& targetPose = trajectory.non_const_front();

    // TODO: remove it
    // just to be sure
    localInStandFoot(targetPose);

    double totalTime = targetPose.time;
    double k = std::min( ((double)getRobotInfo().basicTimeStep) / totalTime, 1.0);

    currentPose.pose = getEngine().interpolate(currentPose.pose, targetPose.pose, k);
    
    // interpolate angles
    double errKnee = targetPose.knee_pitch_offset - currentPose.knee_pitch_offset;
    currentPose.knee_pitch_offset += k*errKnee;

    double errAnkle = targetPose.ankle_roll_offset - currentPose.ankle_roll_offset;
    currentPose.ankle_roll_offset += k*errAnkle;


    if(targetPose.time <= getRobotInfo().basicTimeStep)
    {
      trajectory.pop_front();
    }
    else
      targetPose.time -= getRobotInfo().basicTimeStep;
    
    // calculate the chest
    bool solved = false;
    InverseKinematic::HipFeetPose c = getEngine().controlCenterOfMass(getMotorJointData(), currentPose.pose, solved, false);

    KinematicChain::LinkID supFoot = KinematicChain::numOfLinks;
    Vector3d lastReqCoM;
    switch(theKickingFoot)
    {
    case KickRequest::right:
    {
      supFoot = KinematicChain::LFoot;
      Pose3D reqSupFoot = lastCoMPose.feet.left;
      reqSupFoot.rotation = RotationMatrix();
      lastReqCoM = reqSupFoot.invert() * lastCoMPose.com.translation;
      c.localInLeftFoot();
      break;
    }
    case KickRequest::left:
    {
      supFoot = KinematicChain::RFoot;
      Pose3D reqSupFoot = lastCoMPose.feet.right;
      reqSupFoot.rotation = RotationMatrix();
      lastReqCoM = reqSupFoot.invert() * lastCoMPose.com.translation;
      c.localInRightFoot();
      break;
    }
    }
    Vector3d adjust = getEngine().balanceCoM(getFrameInfo(), getKinematicChainSensor(), lastReqCoM, supFoot);

    //c.hip.translation.y += adjust.y;
    double maxRotate = Math::fromDegrees(20);
    c.hip.rotateX(Math::clamp(adjust.y, -maxRotate, maxRotate));

    lastCoMPose = currentPose.pose;

    if(theParameters.enableStaticStabilizer && getSupportPolygon().mode != SupportPolygon::NONE)
    {
      getEngine().rotationStabilize(
        getRobotInfo(),
        getGroundContactModel(),
        getInertialSensorData(),
        c.hip, c.feet.left, c.feet.right); // stabilize
    }

    getEngine().solveHipFeetIK(c);
    getEngine().copyLegJoints(getMotorJointData().position);
    getEngine().autoArms(getRobotInfo(), c, getMotorJointData().position);
    //getEngine().feetStabilize(getMotorJointData().position);
    // some direct offsets
    getMotorJointData().position[(theKickingFoot == KickRequest::left)?JointData::LKneePitch:JointData::RKneePitch] += currentPose.knee_pitch_offset;
    getMotorJointData().position[(theKickingFoot == KickRequest::left)?JointData::LAnkleRoll:JointData::RAnkleRoll] += currentPose.ankle_roll_offset;
  }//end if
  
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    getMotorJointData().stiffness[i] = 1.0;
  }
}//end execute


void IKDynamicKickMotion::calculateTrajectory(const MotionRequest& motionRequest)
{
  const unsigned int state_time = getFrameInfo().getTimeSince(state_start_time);

  // HACK:
  bool retract_done = false;

  // decide the state change
  switch(kickState)
  {
    case kick_prepare: 
      if(isStopped()) // first time
      {
        kickState = kick_prepare; // stay
      }
      else if(!isRequested(getMotionRequest()))
      {
        kickState = kick_wrap_up;
      }
      else //if(trajectory.empty())
      {
        kickState = kick_retract;
      }
      break;

    case kick_retract:
      retract_done = true;
      if(!isRequested(getMotionRequest()) && !getMotionRequest().kickRequest.finishKick
         //&& numberOfPreKickSteps >= theParameters.minNumberOfPreKickSteps
         )
      {
        kickState = kick_wrap_up;
      }
      else if(getMotionRequest().kickRequest.finishKick 
              //&& currentTrajectoryState() == preKickState() 
              //&& numberOfPreKickSteps >= theParameters.minNumberOfPreKickSteps
              )
      {
        kickState = kick_execute;
      }
      break;

    case kick_execute:
      if(state_time > 0)
        kickState = kick_wrap_up;
      break;

    case kick_wrap_up:
      // stop the motion
      if(state_time > 0 && trajectory.empty())
      {
        setCurrentState(motion::stopped);
        numberOfPreKickSteps = 0;
        wrap_up_made = false;
        return;
      }
      break;

    default:
      THROW("Unexpected kick state.");
  }//end switch


  // detect a state change
  if(oldKickState != kickState)
  {
    action_done = false;
    state_start_time = getFrameInfo().getTime();
    state_frame_count = 0;
    oldKickState = kickState;
  }//end if

  // by default the motion is always running
  setCurrentState(motion::running);



  /*********************************************
   * prepare the reachability grid
   */

  // TODO: use the real chast tranformation
  //const Pose3D& chest =  getKinematicChainSensor().theLinks[KinematicChain::Torso].M;

  /*
  const double foot_front_offset = 60;
  Pose3D staticChest;
  staticChest.translation.z = getStandHeight();
  staticChest.translation.x = foot_front_offset;

  reachibilityGrid.setTransformation(staticChest);
  reachibilityGrid.setRightFoot(theKickingFoot == KickRequest::right);
  */

  KickPose cP;
  cP.pose = getEngine().getCurrentCoMFeetPose();
  localInStandFoot(cP);

  reachibilityGrid.setTransformation(cP.pose.body());
  reachibilityGrid.setRightFoot(theKickingFoot == KickRequest::right);

  /*********************************************
   * BEGIN: adjust the current kicking request
   */

  // coordinates of the support foot
  const Pose3D& supportFootPose = getKinematicChainSensor().theLinks[(theKickingFoot == KickRequest::left)?KinematicChain::RFoot:KinematicChain::LFoot].M;
  // calculate the ball offset vector
  double ballRadius = 32.5;// TODO: a parameter of NaoInfo?
  double footRadius = 45.0;// TODO: a parameter of NaoInfo?


  // the requested kick pose in the robot coordinates
  Pose3D kickPoseRequest(RotationMatrix::getRotationZ(getMotionRequest().kickRequest.kickDirection), getMotionRequest().kickRequest.kickPoint);

  // transform the kick request in the coordinate system of the current support foot
  kickPoseRequest = supportFootPose.invert()*kickPoseRequest;

  //double ty = kickPoseRequest.translation.y;

  DEBUG_REQUEST("IKDynamicKickMotion:kick_request_geometry",
    FIELD_DRAWING_CONTEXT;
    TRANSLATION(supportFootPose.translation.x, supportFootPose.translation.y);
    ROTATION(supportFootPose.rotation.getZAngle());
  );

  // draw the kick request
  DEBUG_REQUEST("IKDynamicKickMotion:kick_request_geometry",
    PEN("999900", 10);
    Vector3<double> direction(100.0,0.0,0.0);
    Vector3<double> end = kickPoseRequest*direction;
    ARROW(kickPoseRequest.translation.x, 
          kickPoseRequest.translation.y,
          end.x, end.y);
    CIRCLE(kickPoseRequest.translation.x, kickPoseRequest.translation.y, ballRadius);
  );

  Vector3<double> ball_offset(-ballRadius, 0.0, 0.0);
  kickPoseRequest.translate(ball_offset);

  // draw the hiting point on the ball
  DEBUG_REQUEST("IKDynamicKickMotion:kick_request_geometry",
    PEN("000000", 10);
    CIRCLE(kickPoseRequest.translation.x, kickPoseRequest.translation.y, 5);
  );

  Vector3<double> foot_offset(-footRadius, 0.0, 0.0);
  kickPoseRequest.translate(foot_offset);

  // draw the target point of the foot
  DEBUG_REQUEST("IKDynamicKickMotion:kick_request_geometry",
    PEN("999999", 10);
    CIRCLE(kickPoseRequest.translation.x, kickPoseRequest.translation.y, 5);
    CIRCLE(kickPoseRequest.translation.x, kickPoseRequest.translation.y, footRadius);
  );

  // kick strength (prolongation)
  Vector3<double> strength_offset(theParameters.strengthOffset, 0.0, 0.0);
  kickPoseRequest.translate(strength_offset);


  // NOTE: assume the foot has the rotation 0 (!!!)
  // offset from the joint to the foot kick point
  const double fsrOffsetX = 70.1; // mm
  Vector3<double> footPointOffset(fsrOffsetX-5, 0.0, -NaoInfo::FootHeight+8.0);
  kickPoseRequest.translation -= footPointOffset;


  // draw the target point of the foot joint
  DEBUG_REQUEST("IKDynamicKickMotion:kick_request_geometry",
    PEN("990000", 10);
    LINE(kickPoseRequest.translation.x, 
         kickPoseRequest.translation.y,
         kickPoseRequest.translation.x + footPointOffset.x, 
         kickPoseRequest.translation.y + footPointOffset.y);
    CIRCLE(kickPoseRequest.translation.x, kickPoseRequest.translation.y, 5);
  );

  DEBUG_REQUEST("IKDynamicKickMotion:reachability_grid_3d",
    drawReachabilityGrid(kickPoseRequest, false, true);
  );

  DEBUG_REQUEST("IKDynamicKickMotion:reachability_grid_2d",
    drawReachabilityGrid(kickPoseRequest, true, false);
  );

  // STEP 4:
  kickPoseRequest = clampKickPoseInReachableSpace(kickPoseRequest);

  // draw the target point of the foot joint
  DEBUG_REQUEST("IKDynamicKickMotion:kick_request_geometry",
    PEN("FFFF00", 20);
    CIRCLE(kickPoseRequest.translation.x, kickPoseRequest.translation.y, 5);
  );

  DEBUG_REQUEST("IKDynamicKickMotion:kick_request_geometry",
    ROTATION(-supportFootPose.rotation.getZAngle());
    TRANSLATION(-supportFootPose.translation.x, -supportFootPose.translation.y);
  );

  /*********************************************
   * END: adjust the current kicking request
   */


  // execute the action of the current state
  switch(kickState)
  {
    case kick_prepare: action_prepare(); break;
    case kick_retract: 
      if(!retract_done) 
        action_retract(kickPoseRequest); 
      else
        if(trajectory.size() < 2)
        {
          KickPose p = trajectory.back();
          p.time = getRobotInfo().basicTimeStep;
          trajectory.push_back(p);
        }
      break;
    case kick_execute: action_execute(kickPoseRequest); break;
    case kick_wrap_up: action_wrap_up(); break;
    default: THROW("Unexpected kick state.");
  }//end switch



  state_frame_count++;
}//end calculateTrajectory


const KickPose IKDynamicKickMotion::getStandPoseOnOneFoot(bool add_rotation)
{
  KickPose standPose = getStandPose(getStandHeight());
  const Pose3D& standFoot = getStandFoot(standPose);

  double offset = (theKickingFoot == KickRequest::left)?theParameters.shiftOffsetYRight:theParameters.shiftOffsetYLeft;
  
  // shift the body
  standPose.pose.com.translate(Vector3<double>(0, standFoot.translation.y + offset, 0));


  double rotationX = theKickingFoot*Math::fromDegrees(theParameters.basicXRotationOffset);
  if(add_rotation)
  {
    rotationX += theKickingFoot*Math::fromDegrees(theParameters.extendedXRotationOffset);
  }
  standPose.pose.com.rotation = RotationMatrix::getRotationX(rotationX);


  return standPose;
}//end getStandPoseOnOneFoot


void IKDynamicKickMotion::action_prepare()
{
  // TODO: rotation of the body?
  KickPose standPose = getStandPoseOnOneFoot(false);

  localInStandFoot(standPose);

  //ASSERT(theParameters.shiftTime >= 0.0);
  // theParameters.shiftTime;

  double v = theParameters.shiftSpeed; // mm/ms = m/s
  double d = (currentPose.pose.body().translation - standPose.pose.body().translation).abs();
  standPose.time = (unsigned int)(d/v);

  trajectory.push_back(standPose);

  // ...for stabilization
  standPose.time = (unsigned int)theParameters.stabilization_time;
  trajectory.push_back(standPose);


  // lift the foot a little
  KickPose rotatedStandPose = getStandPoseOnOneFoot(true);
  double ballRadius = 32.5;
  getKickingFoot(rotatedStandPose).translation.z += ballRadius;
  rotatedStandPose.time = (unsigned int)theParameters.rotationTime;

  localInStandFoot(rotatedStandPose);
  trajectory.push_back(rotatedStandPose);
}//end action_prepare


void IKDynamicKickMotion::action_retract(const Pose3D& kickPose)
{
  Pose3D preparePose = calculatePreparationPose(kickPose);
  // move back to joint
//  const double foot_front_offset = 60;
//  preparePose.translation.x -= foot_front_offset;

  //if (trajectory.size() > 1) return;
  
  // ATTENTION: rely on that p is localized in the stand foot 
  KickPose p = trajectory.back();

  if(theKickingFoot == KickRequest::left)
  {
//    p.lFootMask = Kinematics::InverseKinematics::MASK_POS;
    p.pose.feet.left = preparePose;
  }
  else if ( theKickingFoot == KickRequest::right )
  {
//    p.rFootMask = Kinematics::InverseKinematics::MASK_POS;
    p.pose.feet.right = preparePose;
  }
  else
  {
    THROW("the kicking foot should be left or right");
  }



  // calculate additional knee offset
  {
  const Pose3D& currentFootPose = (theKickingFoot == KickRequest::left)?p.pose.feet.left: p.pose.feet.right;
  //double v = theParameters.kickSpeed; // mm/ms = m/s
  Vector3<double> d = -(currentFootPose.translation);
  double max_kpo = Math::fromDegrees(theParameters.knee_pitch_offset);
  p.knee_pitch_offset = Math::clamp(d.x/100*max_kpo, 0.0, max_kpo);
  
  
  Vector3<double> dir = -(kickPose.translation - currentFootPose.translation);
  double max_aro = Math::fromDegrees(theParameters.ankle_roll_offset);
  p.ankle_roll_offset = dir.y/60*max_aro;
  }



  // ...the first time
  if (false && state_frame_count == 0)
  {
    p.time = (unsigned int)theParameters.time_for_first_preparation;
    trajectory.push_back(p);
  }

  //if (trajectory.size() < 2)
  {
    const Pose3D& currentFootPose = (theKickingFoot == KickRequest::left)?trajectory.back().pose.feet.left: trajectory.back().pose.feet.right;
    double v = theParameters.retractionSpeed; // mm/ms = m/s
    double d = (preparePose.translation - currentFootPose.translation).abs();
    p.time = (unsigned int)(d/v);

    trajectory.push_back(p);
    numberOfPreKickSteps++;
  }
}//end action_retract

void IKDynamicKickMotion::action_execute(const Pose3D& kickPose)
{
  //if (trajectory.size() > 1) return;
  Vector3<double> targetPosition = kickPose.translation;

  // TODO: rotation of the body?
  KickPose p = getStandPoseOnOneFoot(true);
  localInStandFoot(p);

  Pose3D& kickFoot = theKickingFoot == KickRequest::left ? p.pose.feet.left : p.pose.feet.right;

  //kickFoot.translation = kickPose.translation;
  

//  Pose3D currentPose = (theKickingFoot == KickRequest::left)?trajectory.back().pose.feet.left: trajectory.back().pose.feet.right;

  const Pose3D& currentFootPose = (theKickingFoot == KickRequest::left)?trajectory.back().pose.feet.left: trajectory.back().pose.feet.right;

  //double v = theParameters.kickSpeed; // mm/ms = m/s
  double d = (targetPosition - currentFootPose.translation).abs();
  //p.time = (unsigned int)(d/v);

  double a = theParameters.kickSpeed/1000.0; // mm/ms^2
  double t_end = sqrt(2.0*d/a);

  double v = 0;
  Vector3<double> direction = targetPosition - currentFootPose.translation;
  direction.normalize();
  kickFoot.translation = currentFootPose.translation;
  

  const KickPose& kp = getCurrentPose();
  double ankle_roll_offset_start = kp.ankle_roll_offset;
  double knee_pitch_offset_start = kp.knee_pitch_offset;

  for(double t = getRobotInfo().basicTimeStep; t <= t_end; t += getRobotInfo().basicTimeStep)
  {
    v += a*getRobotInfo().basicTimeStep;
    kickFoot.translation += direction*v*getRobotInfo().basicTimeStep;
    p.time = getRobotInfo().basicTimeStep;
    if(t_end == 0.0)
    {
      p.ankle_roll_offset = 0;
      p.knee_pitch_offset = 0;
    }
    else
    {
      double dt = ((t_end-t)/t_end);
      p.ankle_roll_offset = ankle_roll_offset_start*dt;
      p.knee_pitch_offset = knee_pitch_offset_start*dt;
    }
    trajectory.push_back(p);
  }//end for

  DEBUG_REQUEST("IKDynamicKickMotion:kick_point",
    FIELD_DRAWING_CONTEXT;
    PEN("FF00FF", 20);
    CIRCLE(kickFoot.translation.x, kickFoot.translation.y, 20);
  );

  // wrap-up
  p.time = (unsigned int)theParameters.afterKickTime;//1000;
  trajectory.push_back(p);

//  p.interpolationType = JointAngle;
  numberOfPreKickSteps = 0;
}//end action_execute

void IKDynamicKickMotion::action_wrap_up()
{
  if(!wrap_up_made)
  {
    stopKick();
    wrap_up_made = true;
  }
}//end action_wrap_up


Pose3D IKDynamicKickMotion::calculateKickPoseInSupportFootCoordinates(const Vector3<double>& kickPoint, double direction) const
{
  // the footOffsetY should be the same in stand, kick, and walk.
  double footOffsetY = getEngine().getParameters().footOffsetY + NaoInfo::HipOffsetY;

  // assume the original request is in the hip coordinates
  // get the current pose of the hip
  const Pose3D& hip =  getKinematicChainSensor().theLinks[KinematicChain::Hip].M;

  Pose3D footCoord;

  switch (theKickingFoot)
  {
    case KickRequest::right:
    {
      const Pose3D& lFoot = getKinematicChainSensor().theLinks[KinematicChain::LFoot].M;
      footCoord = hip.local(lFoot);
      footCoord.translate(0, -footOffsetY, 0);
      break;
    }
    case KickRequest::left:
    {
      const Pose3D& rFoot = getKinematicChainSensor().theLinks[KinematicChain::RFoot].M;
      footCoord = hip.local(rFoot);
      footCoord.translate(0, footOffsetY, 0);
      break;
    }
    default:
      // it shouldn't happen
      ASSERT(false);
  }//end switch

  // transform the kick request in the coordinate system of the
  // current support foot
  Pose3D request3D;
  request3D.translation.x = kickPoint.x;
  request3D.translation.y = kickPoint.y;
  request3D.rotation = hip.rotation.invert().rotateZ(direction);
  request3D = footCoord.local(request3D);

  request3D.translation.z = kickPoint.z;
  return request3D;
}//end adjustKickRequest

Vector3<double> IKDynamicKickMotion::adjustKickPoseForKickDirection(const Pose3D& kickPose) const
{
  double direction = kickPose.rotation.getZAngle();
  Vector3<double> hitingPoint = kickPose.translation;
  
  //double footRadius = 40.0;// TODO: a parameter of NaoInfo?
  Vector2<double> offset(theParameters.footRadius, 0);
  offset.rotate(direction);

  hitingPoint.x -= offset.x;
  hitingPoint.y -= offset.y;

  return hitingPoint;
}//end adjustKickPoseForKickDirection

Pose3D IKDynamicKickMotion::clampKickPoseInReachableSpace(const Pose3D& pose) const
{
  // todo
  Vector3<double> kickPoint = calculateKickPointProjection(pose);
  
  Pose3D kickPose = pose;

  kickPose.translation.x = kickPoint.x;
  kickPose.translation.y = kickPoint.y;
  
  return kickPose;
}//end clampKickPoseInReachableSpace

inline bool IKDynamicKickMotion::isRequested(const MotionRequest& motionRequest) const
{
  return motionRequest.id == motion::kick && // request didn't change
         theKickingFoot == motionRequest.kickRequest.kickFoot; // foot didn't change
}//end isRequested


inline Pose3D& IKDynamicKickMotion::getKickingFoot( KickPose& kickPose ) const
{
  return (theKickingFoot == KickRequest::left) ? kickPose.pose.feet.left : kickPose.pose.feet.right;
}//end getKickingFoot

inline const Pose3D& IKDynamicKickMotion::getKickingFoot( const KickPose& kickPose ) const
{
  return (theKickingFoot == KickRequest::left) ? kickPose.pose.feet.left : kickPose.pose.feet.right;
}//end getKickingFoot

inline Pose3D& IKDynamicKickMotion::getStandFoot( KickPose& kickPose ) const
{
  return (theKickingFoot == KickRequest::left) ? kickPose.pose.feet.right : kickPose.pose.feet.left;
}//end getKickingFoot

inline const Pose3D& IKDynamicKickMotion::getStandFoot( const KickPose& kickPose ) const
{
  return (theKickingFoot == KickRequest::left) ? kickPose.pose.feet.right : kickPose.pose.feet.left;
}//end getKickingFoot


inline double IKDynamicKickMotion::getStandHeight() const
{
  return getEngine().getParameters().walk.comHeight + theParameters.hipHeightOffset;
}//end getStandHeight

inline const KickPose& IKDynamicKickMotion::getCurrentPose() const
{
  return (trajectory.empty())?currentPose:trajectory.back();
}//end getCurrentPose

void IKDynamicKickMotion::localInStandFoot( KickPose& kickPose ) const
{
  if(theKickingFoot == KickRequest::left)
    kickPose.pose.localInRightFoot();
  else
    kickPose.pose.localInLeftFoot();
}//end localInStandFoot


void IKDynamicKickMotion::stopKick()
{
  Pose3D oldPose = (theKickingFoot == KickRequest::left)?trajectory.back().pose.feet.left: trajectory.back().pose.feet.right;
  
  // get the foot back to the center
  // TODO: rotation of the body?
  KickPose p = getStandPoseOnOneFoot(false);
  localInStandFoot(p);

  Pose3D& currentFootPose = (theKickingFoot == KickRequest::left)?p.pose.feet.left:p.pose.feet.right;
  currentFootPose.translation.z = 32.5; // ball redius
  
  double v = theParameters.takeBackSpeed; // mm/ms = m/s
  double d = (oldPose.translation - currentFootPose.translation).abs();
  p.time = (unsigned int)(d/v); // was 600

  trajectory.push_back(p);

  // put the foot smoothly to the ground (smooth landing)
  // p.interpolationType = InverseKinematic::None;
  p.time = getRobotInfo().basicTimeStep;
  double numberOfSteps = theParameters.timeToLand/getRobotInfo().basicTimeStep;
  for(double i = 0; i < numberOfSteps; i++)
  {
    double t = i/numberOfSteps;
    currentFootPose.translation.z = (1.0+cos(t*Math::pi))*0.5*32.5;
    trajectory.push_back(p);
  }


  // move to the center
  KickPose q = getStandPose(getStandHeight());
  localInStandFoot(q);
  
  double v_stop = theParameters.shiftSpeed; // mm/ms = m/s
  double d_stop = (p.pose.body().translation - q.pose.body().translation).abs();
  q.time = (unsigned int)(d_stop/v_stop);
  trajectory.push_back(q);
}//end stopKick

Pose3D IKDynamicKickMotion::calculatePreparationPose(const Pose3D& kickPose) const
{
  Pose3D preparePose;// = kickPose;
  // todo
  preparePose.translation = calculateTheLoadPoint(kickPose);

  
  Vector3<double> delta = kickPose.translation - preparePose.translation;
  Vector2<double> deltaX(delta.x, delta.z);
  Vector2<double> deltaY(delta.y, delta.z);

//  if(false && deltaX.abs() > 30)
//    preparePose.rotateY(deltaX.angle());

//  if(deltaY.abs() > 30)
//    preparePose.rotateX(deltaY.angle());
  
  //preparePose.rotateY(-0.2);

  return preparePose;
}//end calculatePreparationPose


double IKDynamicKickMotion::retractionValue(const Pose3D& kickPose, const Vector3<double>& pointCoord) const
{
  // angle deviation
  //const double angle_deviation = Math::fromDegrees(5.0);

  double c0 = 0.999;
  MODIFY("IKDynamicKickMotion:angle_factor", c0);

  double f0_angle = fabs(Math::normalize(
               kickPose.rotation.getZAngle() - 
             Vector2<double>(
               pointCoord.x-kickPose.translation.x,
               pointCoord.y-kickPose.translation.y).angle())
               );

  double f0 = 
      (1-c0) * (pointCoord-kickPose.translation).abs() // distance
    +    c0  * f0_angle; // angle

  return f0;
}//end retractionValue



Vector3<double> IKDynamicKickMotion::calculateTheLoadPoint(const Pose3D& kickPose) const
{
  double maxDistanceToRetract = theParameters.maxDistanceToRetract;

  // calculate the starting point for minimization
  // get current kicking foot
  Pose3D currentPose = (theKickingFoot == KickRequest::left)?trajectory.back().pose.feet.left: trajectory.back().pose.feet.right;
  //currentPose.translation.x += 60;

  Vector3<double> retractionPointWorld(currentPose.translation);
  
  //DEBUG: fix the kick height
  retractionPointWorld.z = 60;
  MODIFY("IKDynamicKickMotion::kick_height", retractionPointWorld.z);
  
  // trajectory to be plotted in 3D viewer
  static std::vector<Vector3<double> > trace;
  static Vector3<double> lastPoint(currentPose.translation);
  if(trace.empty())
  {
    trace.push_back(lastPoint);
  }

  double f0 = retractionValue(kickPose, retractionPointWorld);

  // converts the starting point to the grid coords
  Vector3<int> retractionPointGrid = reachibilityGrid.pointToGridCoordinates(retractionPointWorld);
  
  // if the point is unreachable, then there is something wrong...
  

  DEBUG_REQUEST("IKDynamicKickMotion:load_point",
    FIELD_DRAWING_CONTEXT;
    PEN("FFFFFF",1.0f);
    FILLBOX(retractionPointWorld.x-5,retractionPointWorld.y-5,retractionPointWorld.x+5,retractionPointWorld.y+5);
  );

  Vector3<int> mask[27];
  int i = 0;
  for(int x = -1; x < 2; x++)
    for(int y = -1; y < 2; y++)
      for(int z = -1; z < 2; z++)
  {
    mask[i++]=Vector3<int>(x,y,z);
  };

  double avoidCollisionOffset = 50;
  MODIFY("IKDynamicKickMotion::avoidCollisionOffset", avoidCollisionOffset);

  bool updated = true;
  int iteration = 0;
  while(updated && iteration++ < 24)
  {
    updated = false;

    for(int i = 0; i < 27; i++)
    {
      Vector3<int> neighbourGrid = retractionPointGrid + mask[i];
      if(mask[i].z == 0 && // HACK: keep the height
         reachibilityGrid.gridPointReachable(neighbourGrid))
      {
        Vector3<double> neighbourWorld = reachibilityGrid.gridPointToWorldCoordinates(neighbourGrid);

        if(neighbourWorld.x < -100 ||
           neighbourWorld.y*theKickingFoot < avoidCollisionOffset || // don't collide with other leg
           (neighbourWorld - kickPose.translation).abs() > maxDistanceToRetract
          ) continue;

        double f_tmp = retractionValue(kickPose, neighbourWorld);
        
        if(f_tmp > f0)
        {
          f0 = f_tmp;
          retractionPointGrid = neighbourGrid;
          retractionPointWorld = neighbourWorld;
          updated = true;
          //break;
        }
      }//end if
    }//end for
  }//end while

  DEBUG_REQUEST("IKDynamicKickMotion:load_point",
    PEN("FF0000",1.0f);
    FILLBOX(retractionPointWorld.x-5,retractionPointWorld.y-5,retractionPointWorld.x+5,retractionPointWorld.y+5);
    //CIRCLE(retractionPointWorld.x,retractionPointWorld.y,10);
  );

  DEBUG_REQUEST("IKDynamicKickMotion:retraction_trajectory",
    if(lastPoint != retractionPointWorld)
    {
      trace.push_back(retractionPointWorld);
      lastPoint = retractionPointWorld;
    }

    for(unsigned int k = 1; k < trace.size(); k++)
    {
      LINE_3D("FF0000", trace[k-1], trace[k]);
    }
  );

  return retractionPointWorld;
}//end calculateTheLoadPoint


Vector3<double> IKDynamicKickMotion::calculateKickPointProjection(const Pose3D& kickPoseInWorld) const
{
  Vector3<double> originalPointWorld = kickPoseInWorld.translation;
  Vector3<int> originalPointGrid = reachibilityGrid.pointToGridCoordinates(originalPointWorld);

  // check if the original point is already reachable
  if(reachibilityGrid.gridPointReachable(originalPointGrid))
    return originalPointWorld;
  
  // starting point for the projection of the kickPoint
  Vector3<int> projectionGrid(20, 20, 6);
  Vector3<double> projectionWorld = reachibilityGrid.gridPointToWorldCoordinates(projectionGrid);
  double f0 = (projectionWorld-originalPointWorld).abs();

  DEBUG_REQUEST("IKDynamicKickMotion:kick_request_in_grid",
    FIELD_DRAWING_CONTEXT;
    PEN("00FF00",1.0f);
    FILLBOX(projectionWorld.x-5,projectionWorld.y-5,projectionWorld.x+5,projectionWorld.y+5);
    PEN("999999",1.0f);
  );

  // searching mask
  Vector3<int> mask[8] = 
  {
    Vector3<int>(0,1,0),
    Vector3<int>(1,0,0),
    Vector3<int>(0,-1,0),
    Vector3<int>(-1,0,0),
    Vector3<int>(1,1,0),
    Vector3<int>(-1,-1,0),
    Vector3<int>(-1,1,0),
    Vector3<int>(1,-1,0)
  };
  
  bool updated = true;
  int iteration = 0;
  while(updated && iteration++ < 24)
  {
    updated = false;

    for(unsigned int i = 0; i < 8; i++)
    {
      Vector3<int> neighbourGrid = projectionGrid + mask[i];
      if(reachibilityGrid.gridPointReachable(neighbourGrid))
      {
        Vector3<double> neighbourWorld = reachibilityGrid.gridPointToWorldCoordinates(neighbourGrid);
        double f_tmp = (neighbourWorld-originalPointWorld).abs();
        
        if(f_tmp < f0)
        {
          f0 = f_tmp;
          projectionGrid = neighbourGrid;
          projectionWorld = neighbourWorld;
          updated = true;

          DEBUG_REQUEST("IKDynamicKickMotion:kick_request_in_grid",
            FILLBOX(projectionWorld.x-5,projectionWorld.y-5,projectionWorld.x+5,projectionWorld.y+5);
          );
          //break;
        }
      }//end if
    }//end for
  }//end while

  DEBUG_REQUEST("IKDynamicKickMotion:kick_request_in_grid",
    PEN("FF0000",1.0f);
    FILLBOX(projectionWorld.x-5,projectionWorld.y-5,projectionWorld.x+5,projectionWorld.y+5);
  );

  return projectionWorld;
}//end calculateKickPointProjection



void IKDynamicKickMotion::drawReachabilityGrid(const Pose3D& kickPose, bool draw2D, bool draw3D) const
{
  if(!draw2D && !draw3D) return;
  
  Vector3<double> kickFootPoint = (theKickingFoot == KickRequest::left)?currentPose.pose.feet.left.translation:currentPose.pose.feet.right.translation;
  Vector3<int> kickPointGrid = reachibilityGrid.pointToGridCoordinates(kickFootPoint);
  Vector3<int> point(0,0,kickPointGrid.z);


  std::vector<Vector3<double> > reachablePoints;
  std::vector<double > reachableValues;

  double minValue = -1;
  double maxValue = -1;
if(draw2D){ FIELD_DRAWING_CONTEXT; PEN("000000",0.1);}
  for(point.x = 0; point.x < reachibilityGrid.grid.resolution; point.x++)
  {
    for(point.y = 0; point.y < reachibilityGrid.grid.resolution; point.y++)
    {
      if(reachibilityGrid.gridPointReachable(point))
      {
        Vector3<double> pointInGlobalCoords = reachibilityGrid.gridPointToWorldCoordinates(point);

        double d = retractionValue(kickPose, pointInGlobalCoords);
        
        if(minValue == -1 && maxValue == -1)
        {
          minValue = d;
          maxValue = d;
        }

        minValue = std::min(d,minValue);
        maxValue = std::max(d,maxValue);

        reachablePoints.push_back(pointInGlobalCoords);
        reachableValues.push_back(d);

        if(draw2D)
        {
          PARTICLE(pointInGlobalCoords.x, pointInGlobalCoords.y, 0.5, 5);
        }
      }//end if
    }//end for
  }//end for

  double range = maxValue - minValue;

  
  const Pose3D& supportFootPose = getKinematicChainSensor().theLinks[(theKickingFoot == KickRequest::left)?KinematicChain::RFoot:KinematicChain::LFoot].M;

  for(unsigned int i = 0; i < reachablePoints.size(); i++)
  {
    double c = (range == 0)?0:(reachableValues[i] - minValue)/range;
    
    // 2D drawing
    if(draw2D){
      PEN(DebugDrawings::Color(0,0,c,0.8),0);
      FILLBOX(reachablePoints[i].x-5,
              reachablePoints[i].y-5,
              reachablePoints[i].x+5,
              reachablePoints[i].y+5);
    }//end if

    // 3D drawing
    if(draw3D){
      BOX_3D(DebugDrawings::Color(0,0,c,0.8).toString().c_str(), 10, 10, 10, 
        reachibilityGrid.getTransformation().rotation, supportFootPose*reachablePoints[i]);
    }//end if
  }//end for
}//end drawReachabilityGrid

