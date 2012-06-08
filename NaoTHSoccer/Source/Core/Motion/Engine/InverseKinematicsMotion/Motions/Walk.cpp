/**
* @file Walk.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*
*/

#include "Walk.h"
#include "Walk/ZMPPlanner.h"
#include "Walk/FootTrajectoryGenerator.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"

#include "Tools/Math/MatrixBH.h"
#include "Tools/Math/MatrixBH.cpp"

#include "../InverseKinematicBH.h"

using namespace InverseKinematic;
using namespace naoth;

unsigned int Walk::theStepID = 0;

Walk::Walk()
  :IKMotion(motion::walk),
  theWalkParameters(theParameters.walk),
  theWaitLandingCount(0),
  theUnsupportedCount(0),
  isStopping(false),
  stoppingStepFinished(false)
{
}
  
void Walk::execute(const MotionRequest& motionRequest, MotionStatus& motionStatus)
{
  calculateError();
  updateComObserver();

  {
    if ( !theWalkParameters.enableWaitLanding || !waitLanding() )
    {
      plan(motionRequest);
      theCoMFeetPose = executeStep();
    }

    // remember the pose
    commandPoseBuffer.add(theCoMFeetPose);

    bool solved = false;
    HipFeetPose c = theEngine.controlCenterOfMass(theCoMFeetPose, solved, true);

    PLOT("Walk:com.solved", solved);
    PLOT("Walk:c:hip.z",c.hip.translation.z);

    PLOT("Walk:theCoMFeetPose:left_diff",c.feet.left.translation.y - c.hip.translation.y-50);
    PLOT("Walk:theCoMFeetPose:right_diff",c.feet.right.translation.y - c.hip.translation.y+50);

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


    // apply online stabilization
    if(theWalkParameters.rotationStabilize)
    {
      theEngine.rotationStabilize(c.hip, c.feet.left, c.feet.right);
    }

    theEngine.solveHipFeetIK(c);
    /*
    c.localInHip();
    static RobotDimensions theRobotDimensions;
    InverseKinematicBH::calcLegJoints(c.feet.left, c.feet.right, theMotorJointData, theRobotDimensions, 0.5f);
    */

    theEngine.copyLegJoints(theMotorJointData.position);
    
    // move arms
    //theEngine.autoArms(c, theMotorJointData.position);
    theEngine.gotoArms(c, theMotorJointData.position);

    // force the hip joint
    if (theMotorJointData.position[JointData::LHipRoll] < 0)
      theMotorJointData.position[JointData::LHipRoll] *= theWalkParameters.leftHipRollSingleSupFactor;

    if (theMotorJointData.position[JointData::RHipRoll] > 0)
      theMotorJointData.position[JointData::RHipRoll] *= theWalkParameters.rightHipRollSingleSupFactor;

    PLOT("Walk:RHipRoll",theMotorJointData.position[JointData::RHipRoll]);
    PLOT("Walk:LHipRoll",theMotorJointData.position[JointData::LHipRoll]);

    if(theWalkParameters.stabilizeFeet)
      feetStabilize(theMotorJointData.position);
  }

  updateMotionStatus(motionStatus);
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



  const Vector2d& inertial = theBlackBoard.theInertialModel.orientation;
  const Vector2d& gyro = theBlackBoard.theGyrometerData.data;


  // HACK: small filter...
  static Vector2<double> lastGyro = gyro;
  Vector2<double> filteredGyro = (lastGyro+gyro)*0.5;

  Vector2<double> weight;
  weight.x = 
      theWalkParameters.stabilizeFeetP.x * inertial.x
    + theWalkParameters.stabilizeFeetD.x * filteredGyro.x;

  weight.y = 
      theWalkParameters.stabilizeFeetP.y * inertial.y
    + theWalkParameters.stabilizeFeetD.y * filteredGyro.y;


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

  return !theBlackBoard.theGroundContactModel.leftGroundContactAverage && 
         !theBlackBoard.theGroundContactModel.rightGroundContactAverage;

  static unsigned int noTouchCount = 0;

  if ( theBlackBoard.theSupportPolygon.mode == SupportPolygon::NONE
      && noTouchCount <= theWalkParameters.minFSRProtectionCount )
  {
    noTouchCount ++;
  }
  else if ( theBlackBoard.theSupportPolygon.mode != SupportPolygon::NONE
           && noTouchCount > 0 )
  {
    noTouchCount --;
  }

  if ( !isStopping && canStop() )
  {
    return noTouchCount > theWalkParameters.minFSRProtectionCount;
  }
  else
  {
    return false;
  }
}//end FSRProtection


bool Walk::waitLanding()
{
  if ( currentState != motion::running
      || theBlackBoard.theSupportPolygon.mode == SupportPolygon::NONE )
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
  
  bool leftFootSupportable = theBlackBoard.theSupportPolygon.isLeftFootSupportable();
  bool rightFootSupportable = theBlackBoard.theSupportPolygon.isRightFootSupportable();

  bool unSupporting = (raiseLeftFoot && !rightFootSupportable)
                      || (raiseRightFoot && !leftFootSupportable);
                      
  if(unSupporting)
  {
    theUnsupportedCount++;
  }
  else
  {
    theUnsupportedCount = 0;
  }

  if ( theUnsupportedCount > theWalkParameters.maxUnsupportedCount
    && ( theWalkParameters.maxWaitLandingCount < 0 || theWaitLandingCount < theWalkParameters.maxWaitLandingCount) )
  {
    theWaitLandingCount++;
    return true;
  } else
  {
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

  bool fsrStop = (theWalkParameters.enableFSRProtection && FSRProtection() );
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
    else
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
  if ( stepBuffer.empty() )
  {
    cout<<"walk start"<<endl;
    theCoMFeetPose = theEngine.getCurrentCoMFeetPose();
    ZMPFeetPose currentZMP = theEngine.getPlannedZMPFeetPose();
    
    // TODO: why in the left foot?!
    currentZMP.localInLeftFoot(); 

    currentZMP.zmp.translation.z = theWalkParameters.comHeight;
    Step zeroStep;
    updateParameters(zeroStep, req.character);
    zeroStep.footStep = FootStep(currentZMP.feet, FootStep::NONE);
    int prepareStep = theEngine.controlZMPstart(currentZMP);
    zeroStep.numberOfCyclePerFootStep = prepareStep;
    zeroStep.planningCycle = prepareStep;

    addStep(zeroStep);
    theFootStepPlanner.updateParameters(theParameters);

    // set the stiffness for walking
    for( int i=JointData::RShoulderRoll; i<JointData::numOfJoint; i++)
    {
      theMotorJointData.stiffness[i] = theWalkParameters.stiffness;
    }
  }

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

    theFootStepPlanner.updateParameters(theParameters);

    if ( stepCtr && req.stepControl.stepID == theStepID )
    {
      // step control
      step.footStep = theFootStepPlanner.controlStep(planningStep.footStep, req);
      updateParameters(step, req.character);
      step.samplesSingleSupport = max(1, (int) (req.stepControl.time / theBlackBoard.theRobotInfo.basicTimeStep));
      step.numberOfCyclePerFootStep = step.samplesDoubleSupport + step.samplesSingleSupport;
      step.stepControlling = true;
      step.speedDirection = req.stepControl.speedDirection;
    }
    else
    {
      step.footStep = theFootStepPlanner.nextStep(planningStep.footStep, req);
      if ( !isStopping && theWalkParameters.dynamicStepsize )
        adaptStepSize(step.footStep);
      updateParameters(step, req.character);
    }

    addStep(step);
  }
}

void Walk::planStep()
{
  Step& planningStep = stepBuffer.back();
  ASSERT(planningStep.planningCycle < planningStep.numberOfCyclePerFootStep);
  double zmpOffset = theWalkParameters.ZMPOffsetY + theWalkParameters.ZMPOffsetYByCharacter * (1-planningStep.character);
  double zmpOffsetX = theParameters.hipOffsetX;


  // EXPERIMENTAL: correct the ZMP
  double delay = 1000;
  MODIFY("Walk:executeStep:correction.delay", delay);
  double factorX = 0;
  MODIFY("Walk:executeStep:correction.factorX", factorX);
  double factorY = 0;
  MODIFY("Walk:executeStep:correction.factorY", factorY);

  if(corrections.getNumberOfEntries() > delay)
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
    theParameters.hipOffsetX, 
    zmpOffset, 
    planningStep.planningCycle,
    planningStep.samplesDoubleSupport,
    planningStep.samplesSingleSupport,
    planningStep.extendDoubleSupport);
  
  PLOT("Walk:planStep:zmp.x", zmp.x);
  PLOT("Walk:planStep:zmp.y", zmp.y);
  

  // TODO: change the height?
  theEngine.controlZMPpush(Vector3d(zmp_simple.x, zmp_simple.y, theWalkParameters.comHeight));
  planningStep.planningCycle++;
}

CoMFeetPose Walk::executeStep()
{
  // control the target com
  Vector3d com;
  if ( !theEngine.controlZMPpop(com) || stepBuffer.empty() )
  {
    return theEngine.getCurrentCoMFeetPose();
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
    footSupporting = theBlackBoard.theGroundContactModel.rightGroundContact;//theBlackBoard.theSupportPolygon.isRightFootSupportable();
    break;
  }
  case FootStep::RIGHT:
  {
    liftFoot = &result.feet.right;
    result.feet.left = exeFootStep.supFoot();
    footSupporting = theBlackBoard.theGroundContactModel.leftGroundContact;
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
        int maxExtendSamples = static_cast<int>( theWalkParameters.maxExtendDoubleSupportTime / theBlackBoard.theRobotInfo.basicTimeStep );
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
                                                        theWalkParameters.stepHeight, 
                                                        0, //footPitchOffset
                                                        0, //footYawOffset
                                                        0, //footRollOffset
                                                        0, //theWalkParameters.curveFactor, // not used right now
                                                        executingStep.speedDirection);
    }
    else
    {
      *liftFoot = FootTrajectorGenerator::genTrajectory(exeFootStep.footBegin(),
                                                        exeFootStep.footEnd(),
                                                        executingStep.executingCycle,
                                                        executingStep.samplesDoubleSupport,
                                                        executingStep.samplesSingleSupport,
                                                        executingStep.extendDoubleSupport,
                                                        theWalkParameters.stepHeight,
                                                        0, //theBlackBoard.theInertialModel.orientation.y*footPitchOffset, // footPitchOffset
                                                        0, //footYawOffset
                                                        0, //theBlackBoard.theInertialModel.orientation.x*footRollOffset, // footRollOffset
                                                        0  //theWalkParameters.curveFactor // not used right now
                                                        );
    }
  }

  result.com.translation = com;
  result.com.rotation = calculateBodyRotation(result.feet ,executingStep.bodyPitchOffset);

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
  double rAng = feet.left.rotation.getZAngle();
  double lAng = feet.right.rotation.getZAngle();
  double bodyRotation = Math::calculateMeanAngle(rAng, lAng);
  if (abs(Math::normalizeAngle(bodyRotation - lAng)) > Math::pi_2)
  {
    bodyRotation = Math::normalizeAngle(bodyRotation + Math::pi);
  }
  RotationMatrix rot = RotationMatrix::getRotationZ(bodyRotation);
  rot.rotateY(pitch);
  return rot;
}

void Walk::walk(const WalkRequest& req)
{
  manageSteps(req);
  planStep();
  currentState = motion::running;
}

void Walk::stopWalking()
{
  ////////////////////////////////////////////////////////
  // add one step to get stand pose
  ///////////////////////////////////////////////////////

  if ( currentState == motion::stopped )
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
    CoMFeetPose standPose = getStandPose(theWalkParameters.comHeight);
    standPose.localInLeftFoot();
    Pose3D finalBody = finalLeftFoot * standPose.com;
    // wait for the com stops
    if ( theEngine.controlZMPstop(finalBody.translation) )
    {
      currentState = motion::stopped;
      stepBuffer.clear();
      cout<<"walk stopped (standard)"<<endl;
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

  if ( currentState == motion::stopped )
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
    if ( theEngine.controlZMPstop(finalBody.translation) )
    {
      currentState = motion::stopped;
      stepBuffer.clear();
      cout<<"walk stopped"<<endl;
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

  const unsigned int basicTimeStep = theBlackBoard.theRobotInfo.basicTimeStep;
  
  step.bodyPitchOffset = Math::fromDegrees(theParameters.bodyPitchOffset);
  step.samplesDoubleSupport = max(0, (int) (theWalkParameters.doubleSupportTime / basicTimeStep));
  step.samplesSingleSupport = max(1, (int) (theWalkParameters.singleSupportTime / basicTimeStep));
  int extendDoubleSupportByCharacter = max(0, (int)((theWalkParameters.extendDoubleSupportTimeByCharacter / basicTimeStep)
                                                    *(1-character)));

  ASSERT(extendDoubleSupportByCharacter < step.samplesSingleSupport);
  step.samplesDoubleSupport += extendDoubleSupportByCharacter;
  step.samplesSingleSupport -= extendDoubleSupportByCharacter;
  step.numberOfCyclePerFootStep = step.samplesDoubleSupport + step.samplesSingleSupport;
}//end updateParameters


void Walk::calculateError()
{
  if ( currentState != motion::running )
    return;


  int observerMeasurementDelay = 40;
  int index = std::min(int(observerMeasurementDelay / 10 - 0.5), commandPoseBuffer.getNumberOfEntries() - 1);
  const InverseKinematic::CoMFeetPose& expectedCoMFeetPose = commandPoseBuffer[index];

  Vector3d requested_com;
  Vector3d observed_com;

  // if right support
  if(expectedCoMFeetPose.feet.left.translation.z > expectedCoMFeetPose.feet.right.translation.z)
  {
    const Pose3D& footRef_right = expectedCoMFeetPose.feet.right;
    requested_com = footRef_right.local(expectedCoMFeetPose.com).translation;

    Pose3D footObs = theBlackBoard.theKinematicChain.theLinks[KinematicChain::RFoot].M;
    footObs.translate(0, 0, -NaoInfo::FootHeight);
    footObs.rotation = RotationMatrix(); // assume the foot is flat on the ground
    observed_com = footObs.invert() * theBlackBoard.theKinematicChain.CoM;
  }
  else
  {
    const Pose3D& footRef_left = expectedCoMFeetPose.feet.left;
    requested_com = footRef_left.local(expectedCoMFeetPose.com).translation;

    Pose3D footObs = theBlackBoard.theKinematicChain.theLinks[KinematicChain::LFoot].M;
    footObs.translate(0, 0, -NaoInfo::FootHeight);
    footObs.rotation = RotationMatrix(); // assume the foot is flat on the ground
    observed_com = footObs.invert() * theBlackBoard.theKinematicChain.CoM;
  }
  
  currentComError = requested_com - observed_com;
  com_errors.add(currentComError.abs2());

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


void Walk::updateComObserver()
{
  // parameter
  Vector4f observerProcessDeviation(0.1f, 0.1f, 3.f, 3.f);

  Vector2f observerMeasurementDeviationWhenInstable(20.f, 10.f);
  Vector2f observerMeasurementDeviation(20.f, 20.f);


  if(false && com_errors.getAverage() > 100)
  {
    observerMeasurementDeviation = observerMeasurementDeviationWhenInstable;
  }

  // observer
  float deltaTime = (float)theBlackBoard.theRobotInfo.getBasicTimeStepInSecond();
  static Matrix4x4f cov;

  static const Matrix2x4f c(Vector2f(1, 0), Vector2f(0, 1), Vector2f(), Vector2f());
  static const Matrix4x2f cTransposed = c.transpose();
  static const Matrix4x4f a(Vector4f(1, 0, 0, 0), Vector4f(0, 1, 0, 0),
                            Vector4f(deltaTime, 0, 1, 0), Vector4f(0, deltaTime, 0, 1));
  static const Matrix4x4f aTransponsed = a.transpose();

  cov = a * cov * aTransponsed;

  for(int i = 0; i < 4; ++i)
    cov[i][i] += Math::sqr(observerProcessDeviation[i]);

  Matrix2x2f covPlusSensorCov = c * cov * cTransposed;

  covPlusSensorCov[0][0] += Math::sqr(observerMeasurementDeviation[0]);
  covPlusSensorCov[1][1] += Math::sqr(observerMeasurementDeviation[1]);
  Matrix4x2f kalmanGain = cov * cTransposed * covPlusSensorCov.invert();
  cov -= kalmanGain * c * cov;

  // calculate the correction
  Vector2f innovation((float)currentComError.x, (float)currentComError.y);
  Vector4f correction = kalmanGain * innovation;

  corrections.add(Vector2<double>(correction[0], correction[1]));

  PLOT("Walk:executeStep:correction.x", correction[0]);
  PLOT("Walk:executeStep:correction.y", correction[1]);
}//end updateComObserver


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
    Pose3D lastCom = calculateStableCoMByFeet(lastFeet, Math::fromDegrees(theParameters.bodyPitchOffset));
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
  feet.left.translate(theParameters.hipOffsetX, 0 ,0);
  feet.right.translate(theParameters.hipOffsetX, 0 ,0);
  Pose3D com;
  com.rotation = calculateBodyRotation(feet, pitch);
  com.translation = (feet.left.translation + feet.right.translation) * 0.5;
  com.translation.z = theWalkParameters.comHeight;
  return com;
}//end calculateStableCoMByFeet

void Walk::adaptStepSize(FootStep& step) const
{
  Vector3<double> comRef, comObs;

  if ( theCoMFeetPose.feet.left.translation.z > theCoMFeetPose.feet.right.translation.z )
  {
    comRef = theCoMFeetPose.feet.right.invert() * theCoMFeetPose.com.translation;
    const Pose3D& foot = theBlackBoard.theKinematicChain.theLinks[KinematicChain::RFoot].M;
    comObs = foot.invert() * theBlackBoard.theKinematicChain.CoM;
  }
  else
  {
    comRef = theCoMFeetPose.feet.left.invert() * theCoMFeetPose.com.translation;
    const Pose3D& foot = theBlackBoard.theKinematicChain.theLinks[KinematicChain::LFoot].M;
    comObs = foot.invert() * theBlackBoard.theKinematicChain.CoM;
  }

  Vector3<double> comErr = comRef - comObs;

  PLOT("Walk:adaptStepSize:comErr.x",comErr.x);
  PLOT("Walk:adaptStepSize:comErr.y",comErr.y);
  PLOT("Walk:adaptStepSize:comErr.y",comErr.z);

  double k = -1;
  MODIFY("Walk:adaptStepSize:adaptStepSizeK", k);

  Vector3d comErrG = step.supFoot().rotation * currentComError;

  step.footEnd().translation.x += comErrG.x * k;
  step.footEnd().translation.y += comErrG.y * k;
}//end adaptStepSize
