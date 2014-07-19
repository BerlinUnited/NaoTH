/**
* @file Walk.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrichm, Mellmann</a>
*
*/

#include "Walk.h"
#include "Walk/ZMPPlanner.h"
#include "Walk/FootTrajectoryGenerator.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"

using namespace InverseKinematic;
using namespace naoth;

unsigned int Walk::theStepID = 0;

Walk::Walk()
  :IKMotion(getInverseKinematicsMotionEngineService(), motion::walk, getMotionLock()),
  theWalkParameters(getEngine().getParameters().walk),
  theWaitLandingCount(0),
  theUnsupportedCount(0),
  isStopping(false),
  stoppingStepFinished(false)
{
}
  
void Walk::execute()
{
  calculateError();

  {
    if ( !theWalkParameters.stabilization.enableWaitLanding || !waitLanding() )
    {
      plan(getMotionRequest());
      theCoMFeetPose = executeStep();
    }

    // plot planned trajectory
    PLOT("Walk:theCoMFeetPose:com:x",theCoMFeetPose.com.translation.x);
    PLOT("Walk:theCoMFeetPose:com:y",theCoMFeetPose.com.translation.y);
    PLOT("Walk:theCoMFeetPose:com:z",theCoMFeetPose.com.translation.z);

    PLOT("Walk:theCoMFeetPose:feet.left:x",theCoMFeetPose.feet.left.translation.x);
    PLOT("Walk:theCoMFeetPose:feet.left:y",theCoMFeetPose.feet.left.translation.y);
    PLOT("Walk:theCoMFeetPose:feet.left:z",theCoMFeetPose.feet.left.translation.z);

    PLOT("Walk:theCoMFeetPose:feet.right:x",theCoMFeetPose.feet.right.translation.x);
    PLOT("Walk:theCoMFeetPose:feet.right:y",theCoMFeetPose.feet.right.translation.y);
    PLOT("Walk:theCoMFeetPose:feet.right:z",theCoMFeetPose.feet.right.translation.z);

    PLOT("Walk:theCoMFeetPose:feet.left:rotationX",theCoMFeetPose.feet.left.rotation.getXAngle());
    PLOT("Walk:theCoMFeetPose:feet.left:rotationY",theCoMFeetPose.feet.left.rotation.getYAngle());

    PLOT("Walk:theCoMFeetPose:feet.right:rotationX",theCoMFeetPose.feet.right.rotation.getXAngle());
    PLOT("Walk:theCoMFeetPose:feet.right:rotationY",theCoMFeetPose.feet.right.rotation.getYAngle());

    PLOT("Walk:theCoMFeetPose:com:rotationX",theCoMFeetPose.com.rotation.getXAngle());
    PLOT("Walk:theCoMFeetPose:com:rotationY",theCoMFeetPose.com.rotation.getYAngle());
    PLOT("Walk:theCoMFeetPose:com:rotationZ",theCoMFeetPose.com.rotation.getZAngle());

    if(!stepBuffer.empty()) 
    {
      // HACK:
      CoMFeetPose tmp(theCoMFeetPose);
      if(stepBuffer.front().footStep.liftingFoot() == FootStep::LEFT) 
      {
        tmp.localInRightFoot();
        theCoMFeetPose.com.translation.z += theWalkParameters.hip.comHeightOffset * tmp.feet.left.translation.z;
        theCoMFeetPose.com.rotateX( theWalkParameters.hip.comRotationOffsetX *tmp.feet.left.translation.z );

        PLOT("Walk:theCoMFeetPose:total_rotationY",tmp.com.rotation.getYAngle());
      } 
      else if(stepBuffer.front().footStep.liftingFoot() == FootStep::RIGHT) 
      {
        tmp.localInLeftFoot();
        theCoMFeetPose.com.translation.z += theWalkParameters.hip.comHeightOffset * tmp.feet.right.translation.z;
        theCoMFeetPose.com.rotateX( -theWalkParameters.hip.comRotationOffsetX*tmp.feet.right.translation.z );

        PLOT("Walk:theCoMFeetPose:total_rotationY",tmp.com.rotation.getYAngle());
      }

      // buffer the pose
      commandFootIdBuffer.add(stepBuffer.front().footStep.liftingFoot());
      commandPoseBuffer.add(theCoMFeetPose);
    }

    bool solved = false;
    HipFeetPose c = getEngine().controlCenterOfMass(getMotorJointData(), theCoMFeetPose, solved, false);

    PLOT("Walk:com.solved", solved);
    PLOT("Walk:c:hip.z",c.hip.translation.z);

    PLOT("Walk:theCoMFeetPose:left_diff",c.feet.left.translation.y - c.hip.translation.y-50);
    PLOT("Walk:theCoMFeetPose:right_diff",c.feet.right.translation.y - c.hip.translation.y+50);


    // apply online stabilization
    if(getCalibrationData().calibrated && theWalkParameters.stabilization.rotationStabilize)
    {
      /*
      getEngine().rotationStabilize(
        getRobotInfo(),
        getGroundContactModel(),
        getInertialSensorData(),
        c.hip);*/
      
      if(stepBuffer.front().footStep.liftingFoot() == FootStep::LEFT) {
        c.localInRightFoot();
      } else if(stepBuffer.front().footStep.liftingFoot() == FootStep::RIGHT) {
        c.localInLeftFoot();
      } else {
        c.localInHip();
      }
      
      getEngine().rotationStabilize(
        getGyrometerData(),
        getRobotInfo().getBasicTimeStepInSecond(),
        c);
    }

    PLOT("Walk:HipFeetPose:hip:rotationX",c.hip.rotation.getXAngle());
    PLOT("Walk:HipFeetPose:hip:rotationY",c.hip.rotation.getYAngle());
    PLOT("Walk:HipFeetPose:hip:rotationZ",c.hip.rotation.getZAngle());

    PLOT("Walk:HipFeetPose:feet.right:rotationX",c.feet.right.rotation.getXAngle());
    PLOT("Walk:HipFeetPose:feet.right:rotationY",c.feet.right.rotation.getYAngle());

    PLOT("Walk:HipFeetPose:feet.left:rotationX",c.feet.left.rotation.getXAngle());
    PLOT("Walk:HipFeetPose:feet.left:rotationY",c.feet.left.rotation.getYAngle());

    getEngine().solveHipFeetIK(c);

    getEngine().copyLegJoints(getMotorJointData().position);
    
    // move arms
    //getEngine().autoArms(c, getMotorJointData().position);

    /*
    if(getEngine().getParameters().arm.takeBack) {
      getEngine().armsOnBack(getRobotInfo(), c, getMotorJointData().position);
    } else {
      getEngine().gotoArms(
        getMotionStatus(),
        getInertialModel(),
        getRobotInfo(),
        c, getMotorJointData().position);
    }
    */
	
    // force the hip joint
    if (getMotorJointData().position[JointData::LHipRoll] < 0) {
      getMotorJointData().position[JointData::LHipRoll] *= theWalkParameters.general.hipRollSingleSupFactorLeft;
    }
    if (getMotorJointData().position[JointData::RHipRoll] > 0) {
      getMotorJointData().position[JointData::RHipRoll] *= theWalkParameters.general.hipRollSingleSupFactorRight;
    }

    PLOT("Walk:RHipRoll",getMotorJointData().position[JointData::RHipRoll]);
    PLOT("Walk:LHipRoll",getMotorJointData().position[JointData::LHipRoll]);

    if(theWalkParameters.stabilization.stabilizeFeet) {
      feetStabilize(getMotorJointData().position);
    }
  }

  updateMotionStatus(getMotionStatus());
}//end execute



void Walk::feetStabilize(double (&position)[naoth::JointData::numOfJoint])
{
  // calculate the cycle
  // the same as in "FootTrajectorGenerator::genTrajectory"
  
  // no stabilization if there are no steps planned
  if(stepBuffer.empty()) return;

  const Step& executingStep = stepBuffer.front();
  double doubleSupportEnd = executingStep.samplesDoubleSupport / 2 + executingStep.extendDoubleSupport;
  double doubleSupportBegin = executingStep.samplesDoubleSupport / 2 + executingStep.samplesSingleSupport;
  double samplesSingleSupport = executingStep.samplesSingleSupport - executingStep.extendDoubleSupport;
  double cycle = executingStep.executingCycle;
  double z = 0;

  if (cycle <= doubleSupportEnd)
  {
    z = 0;
  }
  else if (cycle <= doubleSupportBegin)
  {
    double t = 1 - (doubleSupportBegin - cycle) / samplesSingleSupport;
    t = t * Math::pi - Math::pi_2; // scale t
    z = (1 + cos(t * 2))*0.5;
  }
  else
  {
    z = 0;
  }

  PLOT("Walk:feetStabilize:z", z);



  const Vector2d& inertial = getInertialModel().orientation;
  const Vector3d& gyro = getGyrometerData().data;


  // HACK: small filter...
  static Vector3d lastGyro = gyro;
  Vector3d filteredGyro = (lastGyro+gyro)*0.5;

  Vector2d weight;
  weight.x = 
      theWalkParameters.stabilization.stabilizeFeetP.x * inertial.x
    + theWalkParameters.stabilization.stabilizeFeetD.x * filteredGyro.x;

  weight.y = 
      theWalkParameters.stabilization.stabilizeFeetP.y * inertial.y
    + theWalkParameters.stabilization.stabilizeFeetD.y * filteredGyro.y;


  switch(executingStep.footStep.liftingFoot())
  {
  case FootStep::LEFT: 
    // adjust left
    position[JointData::LAnkleRoll] -= inertial.x*z;
    position[JointData::LAnklePitch] -= inertial.y*z;

    // stabilize right
    position[JointData::RAnkleRoll] += weight.x*z;
    position[JointData::RAnklePitch] += weight.y*z;
    break;

  case FootStep::RIGHT:
    // adjust right
    position[JointData::RAnkleRoll] -= inertial.x*z;
    position[JointData::RAnklePitch] -= inertial.y*z;

    // stabilize left
    position[JointData::LAnkleRoll] += weight.x*z;
    position[JointData::LAnklePitch] += weight.y*z;
    break;
  default: break; // don't stabilize in double support mode
  };

  lastGyro = gyro;
}//end feetStabilize




bool Walk::FSRProtection()
{
  // no foot on the ground,
  // both feet should on the ground, e.g canStop
  // TODO: check current of leg joints
  // ==> stop walking

  return !getGroundContactModel().leftGroundContactAverage && 
         !getGroundContactModel().rightGroundContactAverage;

  static unsigned int noTouchCount = 0;

  if ( getSupportPolygon().mode == SupportPolygon::NONE
      && noTouchCount <= theWalkParameters.stabilization.minFSRProtectionCount )
  {
    noTouchCount ++;
  }
  else if ( getSupportPolygon().mode != SupportPolygon::NONE
           && noTouchCount > 0 )
  {
    noTouchCount --;
  }

  if ( !isStopping && canStop() ) {
    return noTouchCount > theWalkParameters.stabilization.minFSRProtectionCount;
  } else {
    return false;
  }
}//end FSRProtection


bool Walk::waitLanding()
{
  if ( isStopped()
      || getSupportPolygon().mode == SupportPolygon::NONE )
  {
    theUnsupportedCount = 0;
    return false;
  }

  double leftH = theCoMFeetPose.feet.left.translation.z;
  double rightH = theCoMFeetPose.feet.right.translation.z;
  bool raiseLeftFoot = leftH > 0.1 && leftH > rightH;
  bool raiseRightFoot = rightH > 0.1 && rightH > leftH;

  // don't raise two feet
  ASSERT( !(raiseLeftFoot && raiseRightFoot) );
  
  bool leftFootSupportable = getSupportPolygon().isLeftFootSupportable();
  bool rightFootSupportable = getSupportPolygon().isRightFootSupportable();

  bool unSupporting = (raiseLeftFoot && !rightFootSupportable)
                      || (raiseRightFoot && !leftFootSupportable);
                      
  if(unSupporting) {
    theUnsupportedCount++;
  } else {
    theUnsupportedCount = 0;
  }

  if ( theUnsupportedCount > theWalkParameters.stabilization.maxUnsupportedCount
    && ( theWalkParameters.stabilization.maxWaitLandingCount < 0 || theWaitLandingCount < theWalkParameters.stabilization.maxWaitLandingCount) )
  {
    theWaitLandingCount++;
    return true;
  } else {
    theWaitLandingCount = 0;
    return false;
  }
}//end waitLanding

bool Walk::canStop() const
{
  if (isStopping || stepBuffer.empty() )
    return true;
    
  // wait until full step finished
  const Step& planningStep = stepBuffer.back();
  return planningStep.planningCycle >= planningStep.numberOfCyclePerFootStep;
}

void Walk::plan(const MotionRequest& motionRequest)
{
  WalkRequest walkRequest = motionRequest.walkRequest;
  MODIFY("walk.character", walkRequest.character);

  ASSERT(walkRequest.character<=1);
  ASSERT(walkRequest.character>=0);
  ASSERT(!Math::isNan(walkRequest.target.translation.x));
  ASSERT(!Math::isNan(walkRequest.target.translation.y));
  ASSERT(!Math::isNan(walkRequest.target.rotation));
  

  double emergency_stop = 500;
  MODIFY("Walk:emergency_stop", emergency_stop);

  bool fsrStop = (theWalkParameters.stabilization.enableFSRProtection && FSRProtection() );
  PLOT("Walk:FSRStop", fsrStop);

  if ( (motionRequest.id == getId() && com_errors.getAverage() < emergency_stop && !fsrStop) || !canStop() )
  {
    walk(walkRequest);
    isStopping = false;
    stoppingStepFinished = false;
  }
  else
  {
    if (motionRequest.standardStand) // should end with typical stand
    {
      stopWalking();
    }
    else // stop as you are
    {
      stopWalkingWithoutStand();
    }
  }

}//end plan

void Walk::addStep(const Step& step)
{
  stepBuffer.push_back(step);
  theStepID++;
}

void Walk::manageSteps(const WalkRequest& req)
{
  // first step
  if ( stepBuffer.empty() )
  {
    std::cout << "walk start" << std::endl;
    theCoMFeetPose = getEngine().getCurrentCoMFeetPose();

    ZMPFeetPose currentZMP = getEngine().getPlannedZMPFeetPose();
    currentZMP.localInLeftFoot(); // TODO: why in the left foot?!
    currentZMP.zmp.translation.z = theWalkParameters.hip.comHeight;


    Step zeroStep;
    updateParameters(zeroStep, req.character);
    zeroStep.footStep = FootStep(currentZMP.feet, FootStep::NONE);
    int prepareStep = getEngine().controlZMPstart(currentZMP);
    zeroStep.numberOfCyclePerFootStep = prepareStep;
    zeroStep.planningCycle = prepareStep;

    addStep(zeroStep);
    theFootStepPlanner.updateParameters(getEngine().getParameters());

    // set the stiffness for walking
    for( int i=JointData::RShoulderRoll; i<JointData::numOfJoint; i++) {
      getMotorJointData().stiffness[i] = theWalkParameters.general.stiffness;
    }
    // HACK: turn off the hands
    getMotorJointData().stiffness[JointData::LHand] = -1;
    getMotorJointData().stiffness[JointData::RHand] = -1;
  }// if ( stepBuffer.empty() )


  Step& planningStep = stepBuffer.back();
  if ( planningStep.planningCycle >= planningStep.numberOfCyclePerFootStep )
  {
    // this step is planned completely
    // new foot step
    Step step;
    bool stepCtr = false;
    switch (stepBuffer.back().footStep.liftingFoot())
    {
    case FootStep::NONE:
      stepCtr = true;
      break;
    case FootStep::LEFT:
      stepCtr = !req.stepControl.moveLeftFoot;
      break;
    case FootStep::RIGHT:
      stepCtr = req.stepControl.moveLeftFoot;
      break;
    default: ASSERT(false);
      break;
    }

    theFootStepPlanner.updateParameters(getEngine().getParameters());

    if ( stepCtr && req.stepControl.stepID == theStepID )
    {
      // step control
      step.footStep = theFootStepPlanner.controlStep(planningStep.footStep, req);
      updateParameters(step, req.character);
      step.samplesSingleSupport = std::max(1, (int) (req.stepControl.time / getRobotInfo().basicTimeStep));
      step.numberOfCyclePerFootStep = step.samplesDoubleSupport + step.samplesSingleSupport;
      step.stepControlling = true;
      step.speedDirection = req.stepControl.speedDirection;
      step.scale = req.stepControl.scale;
    }
    else
    {
      step.footStep = theFootStepPlanner.nextStep(planningStep.footStep, req);
      PLOT("Walk:step.footStep.rotY", step.footStep.end().left.rotation.getYAngle());
      PLOT("Walk:step.footStep.rotX", step.footStep.end().left.rotation.getXAngle());

      if ( !isStopping && theWalkParameters.stabilization.dynamicStepsize ) {
        adaptStepSize(step.footStep);
        currentComErrorBuffer.clear();
      }
      updateParameters(step, req.character);
    }

    addStep(step);
  }
}//end manageSteps


void Walk::planStep()
{
  Step& planningStep = stepBuffer.back();
  ASSERT(planningStep.planningCycle < planningStep.numberOfCyclePerFootStep);
  double zmpOffset = theWalkParameters.hip.ZMPOffsetY + theWalkParameters.hip.ZMPOffsetYByCharacter * (1-planningStep.character);
  double zmpOffsetX = getEngine().getParameters().hipOffsetX;


  // EXPERIMENTAL: correct the ZMP
  double delay = 1000;
  MODIFY("Walk:executeStep:correction.delay", delay);
  double factorX = 0;
  MODIFY("Walk:executeStep:correction.factorX", factorX);
  double factorY = 0;
  MODIFY("Walk:executeStep:correction.factorY", factorY);

  if(corrections.size() > delay)
  {
    zmpOffsetX += fabs(corrections[(int)delay].x)*factorX;
    zmpOffset  += fabs(corrections[(int)delay].y)*factorY;

    PLOT("Walk:executeStep:correction_delay.x", corrections[(int)delay].x);
    PLOT("Walk:executeStep:correction_delay.y", corrections[(int)delay].y);
  }


  Vector2d zmp_simple = ZMPPlanner::simplest(planningStep.footStep, zmpOffsetX, zmpOffset);
  
  PLOT("Walk:planStep:zmp_simple.x", zmp_simple.x);
  PLOT("Walk:planStep:zmp_simple.y", zmp_simple.y);

  
  // better ZMP?
  Vector2d zmp = ZMPPlanner::betterOne(
    planningStep.footStep, 
    getEngine().getParameters().hipOffsetX, 
    zmpOffset, 
    planningStep.planningCycle,
    planningStep.samplesDoubleSupport,
    planningStep.samplesSingleSupport,
    planningStep.extendDoubleSupport);
  
  PLOT("Walk:planStep:zmp.x", zmp.x);
  PLOT("Walk:planStep:zmp.y", zmp.y);


  // TODO: change the height?
  getEngine().controlZMPpush(Vector3d(zmp_simple.x, zmp_simple.y, theWalkParameters.hip.comHeight));
  planningStep.planningCycle++;
}

CoMFeetPose Walk::executeStep()
{
  // control the target com
  // don't do anything until there are enough steps to plan the next com
  // i.e., the PreviewController is ready
  Vector3d com;
  if ( !getEngine().controlZMPpop(com) || stepBuffer.empty() )
  {
    //return getEngine().getCurrentCoMFeetPose();
    return theCoMFeetPose;
  }

  PLOT("Walk:executeStep:com.x", com.x);
  PLOT("Walk:executeStep:com.y", com.y);
  PLOT("Walk:executeStep:com.z", com.z);


  Step& executingStep = stepBuffer.front();
  ASSERT(executingStep.executingCycle < executingStep.planningCycle);

  FootStep& exeFootStep = executingStep.footStep;
  Pose3D* liftFoot = NULL;

  CoMFeetPose result;
  bool footSupporting;
  switch(exeFootStep.liftingFoot())
  {
  case FootStep::LEFT:
  {
    liftFoot = &result.feet.left;
    result.feet.right = exeFootStep.supFoot();
    footSupporting = getGroundContactModel().rightGroundContact;//getSupportPolygon.isRightFootSupportable();
    break;
  }
  case FootStep::RIGHT:
  {
    liftFoot = &result.feet.right;
    result.feet.left = exeFootStep.supFoot();
    footSupporting = getGroundContactModel().leftGroundContact;
    break;
  }
  case FootStep::NONE:
  {
    result.feet = exeFootStep.begin();
    break;
  }
  default: ASSERT(false);
  }

  if ( liftFoot != NULL )
  {

    // checking on when the foot is lifting
    if ( !executingStep.lifted ) // the foot is on the ground now
    {
      double doubleSupportEnd = executingStep.samplesDoubleSupport / 2 + executingStep.extendDoubleSupport;
      if( executingStep.executingCycle > doubleSupportEnd ) // want to lift the foot
      {
        int maxExtendSamples = static_cast<int>( theWalkParameters.step.maxExtendDoubleSupportTime / getRobotInfo().basicTimeStep );
        if( !footSupporting // but another foot can not support
            && executingStep.extendDoubleSupport < maxExtendSamples ) // allow to extend double support
        {
          executingStep.extendDoubleSupport++;
        }
        else
        {
          executingStep.lifted = true;
          //cout<<"extend "<<executingStep.extendDoubleSupport<<"/"<<maxExtendSamples<<endl;
        }
      }
    }

    double footPitchOffset = 1;
    MODIFY("_test:footPitchOffset", footPitchOffset);
    double footRollOffset = -1;
    MODIFY("_test:footRollOffset", footRollOffset);

    if ( executingStep.stepControlling )
    {
      *liftFoot = FootTrajectorGenerator::stepControl(  exeFootStep.footBegin(),
                                                        exeFootStep.footEnd(),
                                                        executingStep.executingCycle,
                                                        executingStep.samplesDoubleSupport,
                                                        executingStep.samplesSingleSupport,
                                                        executingStep.extendDoubleSupport,
                                                        theWalkParameters.step.stepHeight, 
                                                        0, //footPitchOffset
                                                        0, //footRollOffset
                                                        //theWalkParameters.curveFactor, // not used right now
                                                        executingStep.speedDirection,
                                                        executingStep.scale);
      /*
      StepControlTrajectory trajectory(
        exeFootStep.footBegin(),
        exeFootStep.footEnd(),
        executingStep.samplesDoubleSupport,
        executingStep.samplesSingleSupport,
        executingStep.extendDoubleSupport,
        theWalkParameters.step.stepHeight,
        executingStep.speedDirection);

      *liftFoot = trajectory(executingStep.executingCycle);*/
    }
    else
    {
      *liftFoot = FootTrajectorGenerator::genTrajectory(exeFootStep.footBegin(),
                                                        exeFootStep.footEnd(),
                                                        executingStep.executingCycle,
                                                        executingStep.samplesDoubleSupport,
                                                        executingStep.samplesSingleSupport,
                                                        executingStep.extendDoubleSupport,
                                                        theWalkParameters.step.stepHeight,
                                                        0, // footPitchOffset
                                                        0  // footRollOffset
                                                        // theWalkParameters.curveFactor // not used right now
                                                        );
      /*
      WalkTrajectory trajectory(
        exeFootStep.footBegin(),
        exeFootStep.footEnd(),
        executingStep.samplesDoubleSupport,
        executingStep.samplesSingleSupport,
        executingStep.extendDoubleSupport,
        theWalkParameters.step.stepHeight);

      *liftFoot = trajectory(executingStep.executingCycle);*/
    }
  }

  result.com.translation = com;
  //result.localInLeftFoot();
  result.com.rotation = calculateBodyRotation(result.feet, executingStep.bodyPitchOffset);
  
  executingStep.executingCycle++;
  if ( executingStep.executingCycle >= executingStep.numberOfCyclePerFootStep )
  {
    ASSERT( stepBuffer.size() > 1);
    // this step is executed
    stepBuffer.pop_front();
    //theCoMErr /= numberOfCyclePerFootStep;
    //theCoMErr = Vector3d();
  }

  return result;
}//end executeStep


RotationMatrix Walk::calculateBodyRotation(const FeetPose& feet, double pitch) const
{
  double bodyAngleZ = Math::meanAngle(feet.left.rotation.getZAngle(), feet.right.rotation.getZAngle());
  return RotationMatrix::getRotationZ(bodyAngleZ) * RotationMatrix::getRotationY(pitch);
}

void Walk::walk(const WalkRequest& req)
{
  manageSteps(req);
  planStep();
  setCurrentState(motion::running);
}

void Walk::stopWalking()
{
  ////////////////////////////////////////////////////////
  // add one step to get stand pose
  ///////////////////////////////////////////////////////

  if ( isStopped() )
    return;

  if ( !isStopping ) // remember the stopping foot
  {
    /*
    switch (currentFootStep.liftingFoot()) {
    case FootStep::LEFT:
      stoppingRequest.coordinate = WalkRequest::LFoot;
      break;
    case FootStep::RIGHT:
      stoppingRequest.coordinate = WalkRequest::RFoot;
      break;
    }*/

    stoppingRequest.coordinate = WalkRequest::Hip;
    stoppingRequest.target = Pose2D();
  }

  if ( !stoppingStepFinished )
  {
    // make stopping step
    manageSteps(stoppingRequest);

    const Step& planningStep = stepBuffer.back();
    if ( planningStep.planningCycle == 0 )
    {
      Pose3D diff = planningStep.footStep.footBegin().invert() * planningStep.footStep.footEnd();
      if ( diff.translation.abs2() < 1 && diff.rotation.getZAngle() < Math::fromDegrees(1) )
      {
        // don't need to move the foot
        FootStep zeroStep(planningStep.footStep.end(), FootStep::NONE);
        Step& lastStep = stepBuffer.back();
        lastStep.footStep = zeroStep;
        lastStep.numberOfCyclePerFootStep = 0;
        stoppingStepFinished = true;
      }
    }
  }

  if ( !stoppingStepFinished )
  {
    planStep();
  }
  else
  {
    Pose3D finalLeftFoot = stepBuffer.back().footStep.end().left;
    CoMFeetPose standPose = getStandPose(theWalkParameters.hip.comHeight);
    standPose.localInLeftFoot();
    Pose3D finalBody = finalLeftFoot * standPose.com;
    // wait for the com stops
    if ( getEngine().controlZMPstop(finalBody.translation) )
    {
      setCurrentState(motion::stopped);
      stepBuffer.clear();
      std::cout<<"walk stopped (standard)"<<std::endl;
    }
    else
    {
      Step& lastStep = stepBuffer.back();
      lastStep.planningCycle++;
      lastStep.numberOfCyclePerFootStep = lastStep.planningCycle;
    }
  }

  isStopping = true;
}

void Walk::stopWalkingWithoutStand()
{
  ////////////////////////////////////////////////////////
  // add one step to get stand pose
  ///////////////////////////////////////////////////////

  if ( isStopped() )
    return;

  if ( !isStopping ) // remember the stopping foot
  {
    stoppingRequest.coordinate = WalkRequest::Hip;
    stoppingRequest.target = Pose2D();
  }

  if ( !stoppingStepFinished )
  {
    // make stopping step
    manageSteps(stoppingRequest);

    const Step& planningStep = stepBuffer.back();
    if ( planningStep.planningCycle == 0 )
    {
      // just modify last planning step
      FootStep zeroStep(planningStep.footStep.begin(), FootStep::NONE);
      Step& lastStep = stepBuffer.back();
      lastStep.footStep = zeroStep;
      lastStep.numberOfCyclePerFootStep = 0;
      stoppingStepFinished = true;
    }
  }

  if ( !stoppingStepFinished )
  {
    planStep();
  }
  else
  {
    const Step& finalStep = stepBuffer.back();
    const FeetPose& finalFeet = finalStep.footStep.end();
    Pose3D finalBody = calculateStableCoMByFeet(finalFeet, finalStep.bodyPitchOffset);

    // wait for the com stops
    if ( getEngine().controlZMPstop(finalBody.translation) )
    {
      setCurrentState(motion::stopped);
      stepBuffer.clear();
      std::cout<<"walk stopped"<<std::endl;
    }
    else
    {
      Step& lastStep = stepBuffer.back();
      lastStep.planningCycle++;
      lastStep.numberOfCyclePerFootStep = lastStep.planningCycle;
    }
  }

  isStopping = true;
}//end stopWalkingWithoutStand

void Walk::updateParameters(Step& step, double character) const
{
  ASSERT(character<=1);
  ASSERT(character>=0);
  step.character = character;

  const unsigned int basicTimeStep = getRobotInfo().basicTimeStep;
  
  step.bodyPitchOffset = Math::fromDegrees(getEngine().getParameters().bodyPitchOffset);
  step.samplesDoubleSupport = std::max(0, (int) (theWalkParameters.step.doubleSupportTime / basicTimeStep));
  step.samplesSingleSupport = std::max(1, (int) (theWalkParameters.step.singleSupportTime / basicTimeStep));
  int extendDoubleSupportByCharacter = std::max(0, (int)((theWalkParameters.step.extendDoubleSupportTimeByCharacter / basicTimeStep)
                                                    *(1-character)));

  ASSERT(extendDoubleSupportByCharacter < step.samplesSingleSupport);
  step.samplesDoubleSupport += extendDoubleSupportByCharacter;
  step.samplesSingleSupport -= extendDoubleSupportByCharacter;
  step.numberOfCyclePerFootStep = step.samplesDoubleSupport + step.samplesSingleSupport;
}//end updateParameters


void Walk::calculateError()
{
  if ( isStopped() )
    return;


  int observerMeasurementDelay = 40;
  int index = std::min(int(observerMeasurementDelay / 10 - 0.5), int(commandPoseBuffer.size() - 1));
  InverseKinematic::CoMFeetPose expectedCoMFeetPose = commandPoseBuffer[index];

  Vector3d requested_com;
  Vector3d observed_com;

  // if right support
  if(commandFootIdBuffer[index] == FootStep::LEFT)
  {
    //const Pose3D& footRef_right = expectedCoMFeetPose.feet.right;
    //requested_com = footRef_right.local(expectedCoMFeetPose.com).translation;
    expectedCoMFeetPose.localInRightFoot();
    requested_com = expectedCoMFeetPose.com.translation;

    Pose3D footObs = getKinematicChainSensor().theLinks[KinematicChain::RFoot].M;
    footObs.translate(0, 0, -NaoInfo::FootHeight);
    footObs.rotation = RotationMatrix::getRotationY(footObs.rotation.getYAngle()); // assume the foot is flat on the ground
    observed_com = footObs.invert() * getKinematicChainSensor().CoM;
  }
  else
  {
    //const Pose3D& footRef_left = expectedCoMFeetPose.feet.left;
    //requested_com = footRef_left.local(expectedCoMFeetPose.com).translation;
    expectedCoMFeetPose.localInLeftFoot();
    requested_com = expectedCoMFeetPose.com.translation;

    Pose3D footObs = getKinematicChainSensor().theLinks[KinematicChain::LFoot].M;
    footObs.translate(0, 0, -NaoInfo::FootHeight);
    footObs.rotation = RotationMatrix::getRotationY(footObs.rotation.getYAngle()); // assume the foot is flat on the ground
    observed_com = footObs.invert() * getKinematicChainSensor().CoM;
  }
  
  currentComError = requested_com - observed_com;
  com_errors.add(currentComError.abs2());
  currentComErrorBuffer.add(currentComError);

  PLOT("Walk:comErr:x", currentComError.x);
  PLOT("Walk:comErr:y", currentComError.y);
  PLOT("Walk:comErr:z", currentComError.z);

  PLOT("Walk:comErrAverage", com_errors.getAverage());

  PLOT("Walk:observed_com:x", observed_com.x);
  PLOT("Walk:observed_com:y", observed_com.y);
  PLOT("Walk:observed_com:z", observed_com.z);

  PLOT("Walk:requested_com:x", requested_com.x);
  PLOT("Walk:requested_com:y", requested_com.y);
  PLOT("Walk:requested_com:z", requested_com.z);
}//end calculateError


void Walk::updateMotionStatus(MotionStatus& motionStatus)
{
  if ( stepBuffer.empty() )
  {
    motionStatus.plannedMotion.lFoot = Pose2D();
    motionStatus.plannedMotion.rFoot = Pose2D();
    motionStatus.plannedMotion.hip = Pose2D();
  }
  else
  {
    FeetPose lastFeet = stepBuffer.back().footStep.end();
    Pose3D lastCom = calculateStableCoMByFeet(lastFeet, Math::fromDegrees(getEngine().getParameters().bodyPitchOffset));

    Pose3D plannedHip = theCoMFeetPose.com.invert() * lastCom;
    Pose3D plannedlFoot = theCoMFeetPose.feet.left.invert() * lastFeet.left;
    Pose3D plannedrFoot = theCoMFeetPose.feet.right.invert() * lastFeet.right;

    motionStatus.plannedMotion.hip = reduceDimen(plannedHip);
    motionStatus.plannedMotion.lFoot =  reduceDimen(plannedlFoot);
    motionStatus.plannedMotion.rFoot = reduceDimen(plannedrFoot);
  }

  // step control
  motionStatus.stepControl.stepID = theStepID;
  if ( stepBuffer.empty() )
  {
    motionStatus.stepControl.moveableFoot = MotionStatus::StepControlStatus::BOTH;
  }
  else
  {
    FootStep::Foot lastMovingFoot = stepBuffer.back().footStep.liftingFoot();
    switch(lastMovingFoot)
    {
    case FootStep::NONE:
      motionStatus.stepControl.moveableFoot = MotionStatus::StepControlStatus::BOTH;
      break;
    case FootStep::LEFT:
      motionStatus.stepControl.moveableFoot = MotionStatus::StepControlStatus::RIGHT;
      break;
    case FootStep::RIGHT:
      motionStatus.stepControl.moveableFoot = MotionStatus::StepControlStatus::LEFT;
      break;
    default: ASSERT(false);
      break;
    }
  }
}//end updateMotionStatus

Pose3D Walk::calculateStableCoMByFeet(FeetPose feet, double pitch) const
{
  feet.left.translate(getEngine().getParameters().hipOffsetX, 0 ,0);
  feet.right.translate(getEngine().getParameters().hipOffsetX, 0 ,0);
  Pose3D com;
  com.rotation = calculateBodyRotation(feet, pitch);
  com.translation = (feet.left.translation + feet.right.translation) * 0.5;
  com.translation.z = theWalkParameters.hip.comHeight;
  return com;
}//end calculateStableCoMByFeet

void Walk::adaptStepSize(FootStep& step) const
{
  /*
  Vector3d comRef, comObs;

  if ( theCoMFeetPose.feet.left.translation.z > theCoMFeetPose.feet.right.translation.z )
  {
    comRef = theCoMFeetPose.feet.right.invert() * theCoMFeetPose.com.translation;
    const Pose3D& foot = getKinematicChainSensor().theLinks[KinematicChain::RFoot].M;
    comObs = foot.invert() * getKinematicChainSensor().CoM;
  }
  else
  {
    comRef = theCoMFeetPose.feet.left.invert() * theCoMFeetPose.com.translation;
    const Pose3D& foot = getKinematicChainSensor().theLinks[KinematicChain::LFoot].M;
    comObs = foot.invert() * getKinematicChainSensor().CoM;
  }

  Vector3d comErr = comRef - comObs;

  PLOT("Walk:adaptStepSize:comErr.x",comErr.x);
  PLOT("Walk:adaptStepSize:comErr.y",comErr.y);
  PLOT("Walk:adaptStepSize:comErr.z",comErr.z);
  */

  if(currentComErrorBuffer.size() > 0) 
  {
    Vector3d errorCoM = currentComErrorBuffer.getAverage();
    static Vector3d lastCoMError = errorCoM;
    
    Vector3d comCorrection = errorCoM*theWalkParameters.stabilization.dynamicStepsizeP + 
                          (errorCoM - lastCoMError)*theWalkParameters.stabilization.dynamicStepsizeD;

    Vector3d stepCorrection = step.supFoot().rotation * comCorrection;
    step.footEnd().translation.x += stepCorrection.x;
    step.footEnd().translation.y += stepCorrection.y;

    lastCoMError = errorCoM;
  }
}//end adaptStepSize
