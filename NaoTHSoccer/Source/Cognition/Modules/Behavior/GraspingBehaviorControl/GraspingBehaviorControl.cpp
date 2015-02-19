/**
 * @file GraspingBehaviorControl.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * Implementation of class GraspingBehaviorControl
 */

#include "GraspingBehaviorControl.h"

// Tools
#include "Tools/NaoInfo.h"

GraspingBehaviorControl::GraspingBehaviorControl() 
  : 
  sitHeight(220.0)
{

  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:receive_ssd", "", false);
  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:init", "", false);
  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:sit", "", false);
  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:grasp_ball", "", false);
  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:look_at_grasping_point", "", true);


  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:empty", "", false);
  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:open", "", false);
  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:no_dist", "", false);
  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:stiff_eq_pressure", "in this mode the siffness of the left arm is equal to measured pressure.", false);
  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:horizontal_hand", "in this mode the robot tries to hold the left hand horizontally, be changing the elbow arc.", false);
  

  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:test_motion_model", "makes the arms move randomly and compares expected and messured values.", false);
  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:test_roundtrip_delay", "orders the arms to move and plots the ordered and meassured arcs.", false);

  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:DistController:min_dist", "sets the desired hand distance to a minimum", false);
  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:DistController:no_sensor_dist", "..", false);
  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:DistController:thresh_dist_dist", "sets the desired distance to the measured distance minus a constant", false);
  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:DistController:thresh_curr_dist", "uses the threshhold conroler with currency as input and distance as output", false);
  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:DistController:thresh_force_dist", "uses the threshhold conroler with force as input and distance as output", false);

  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:StiffController:max_stiff", "sets the arms stiffness to a maximum", false);
  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:StiffController:integ_curr_stiff", "uses the integral conroler with currency as input and stiffness as output", false);
  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:StiffController:integ_force_stiff", "uses the integral conroler with force as input and stiffness as output", false);
  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:StiffController:p_force_stiff", "", false);

  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:Experiments:take_object_from_table", "", false);
  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:Experiments:track_and_take_object", "", false);

  DEBUG_REQUEST_REGISTER("GraspingBehaviorControl:Grasp:wave", "", false);
}

void GraspingBehaviorControl::execute() 
{
  
  MODIFY("GraspingBehaviorControl:sitHeight", sitHeight);

  // reset motion request
  getMotionRequest().id = motion::stand;
  getMotionRequest().forced = false;
  getMotionRequest().standHeight = -1; //standing

  // set default arm control
  getMotionRequest().armMotionRequest.id = ArmMotionRequest::arms_neutral;


  DEBUG_REQUEST("GraspingBehaviorControl:init", getMotionRequest().id = motion::init; );

  DEBUG_REQUEST("GraspingBehaviorControl:sit",
    getMotionRequest().id = motion::stand;
    getMotionRequest().standHeight = sitHeight; // sit in a stable position
  );


  GraspRequest::GraspingState& graspingState = getMotionRequest().graspRequest.graspingState;
  GraspRequest::GraspDistState& graspDistState = getMotionRequest().graspRequest.graspDistState;
  GraspRequest::GraspStiffState& graspStiffState = getMotionRequest().graspRequest.graspStiffState;

  // set the default values
  graspingState = GraspRequest::none;
  graspDistState = GraspRequest::GDS_none;
  graspStiffState = GraspRequest::GSS_none;

  // change the values if a debug request is checked
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:empty",                graspingState = GraspRequest::empty;);
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:open",                 graspingState = GraspRequest::open;);
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:no_dist",              graspingState = GraspRequest::no_dist;);
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:stiff_eq_pressure",    graspingState = GraspRequest::stiff_eq_pressure;);
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:horizontal_hand",      graspingState = GraspRequest::horizontal_hand;);
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:test_motion_model",    graspingState = GraspRequest::test_motion_model;);
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:test_roundtrip_delay", graspingState = GraspRequest::test_roundtrip_delay;);

  // distance controllers
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:DistController:min_dist",          graspDistState = GraspRequest::min_dist;);
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:DistController:no_sensor_dist",    graspDistState = GraspRequest::no_sensor_dist;);
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:DistController:thresh_dist_dist",  graspDistState = GraspRequest::thresh_dist_dist;);
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:DistController:thresh_curr_dist",  graspDistState = GraspRequest::thresh_curr_dist;);
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:DistController:thresh_force_dist", graspDistState = GraspRequest::thresh_force_dist;);

  // stiffness controllers
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:StiffController:max_stiff",         graspStiffState = GraspRequest::max_stiff;);
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:StiffController:integ_curr_stiff",  graspStiffState = GraspRequest::integ_curr_stiff;);
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:StiffController:integ_force_stiff", graspStiffState = GraspRequest::integ_force_stiff;);
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:StiffController:p_force_stiff",     graspStiffState = GraspRequest::p_force_stiff;);
  

  // default grasping point
  Vector3d defaultGraspingCenter(145, 0, 90);
  const Pose3D& chestPose = getKinematicChain().theLinks[KinematicChain::Torso].M;
  defaultGraspingCenter = chestPose * defaultGraspingCenter;

  getMotionRequest().graspRequest.graspingPoint = defaultGraspingCenter;
  

  DEBUG_REQUEST("GraspingBehaviorControl:look_at_grasping_point",
    getHeadMotionRequest().id = HeadMotionRequest::look_at_world_point;
    getHeadMotionRequest().targetPointInTheWorld = getMotionRequest().graspRequest.graspingPoint;
  );

  // integrated behaviors
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:Experiments:take_object_from_table", take_object_from_table(); );
  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:Experiments:track_and_take_object", track_and_take_object(); );

  

  DEBUG_REQUEST("GraspingBehaviorControl:Grasp:wave",
    double t = getFrameInfo().getTimeInSeconds()*Math::pi;
    double r = 20.0; // mm
    getMotionRequest().graspRequest.graspingPoint.z += sin(t)*r;
  );

  // turn off arm control if grasping is on
  if(graspingState != GraspRequest::none) {
    getMotionRequest().armMotionRequest.id = ArmMotionRequest::arms_none;
  }

}//end execute

void GraspingBehaviorControl::take_object_from_table()
{
  static int state = 0;
  static int old_state = state;
  static double state_start_time = getFrameInfo().getTimeInSeconds(); // in s

  //to reset the state, when DEBUG-REQUEST isn't selected
  static unsigned int oldFrameNumber = getFrameInfo().getFrameNumber();
  if (getFrameInfo().getFrameNumber() - oldFrameNumber > 1) 
  {
    state = 0;
  }

  // state changed
  if(state != old_state)
  {
    old_state = state;
    state_start_time = getFrameInfo().getTimeInSeconds();
  }

  // the time of the curren state
  double state_time = getFrameInfo().getTimeInSeconds() - state_start_time;

  //to set graspingState
  //GraspRequest::GraspingState& graspingState = GraspRequest::none;
  GraspRequest::GraspingState& graspingState = getMotionRequest().graspRequest.graspingState;
  GraspRequest::GraspDistState& graspDistState = getMotionRequest().graspRequest.graspDistState;
  GraspRequest::GraspStiffState& graspStiffState = getMotionRequest().graspRequest.graspStiffState;

  double timeForSitState = 1.0; // s
  MODIFY("GraspingBehaviorControl:take_object_from_table:timeForSitState", timeForSitState);
  double timeForGraspState = 1.0; //s
  MODIFY("GraspingBehaviorControl:take_object_from_table:timeForGraspState", timeForGraspState);

  double comHeight = getKinematicChain().CoM.z; 

  Vector3d defaultGraspingCenter(145, 0, 90);
  const Pose3D& chestPose = getKinematicChain().theLinks[KinematicChain::Torso].M;
  defaultGraspingCenter = chestPose * defaultGraspingCenter;
  getMotionRequest().graspRequest.graspingPoint = defaultGraspingCenter;

  //minimal height while sitting is 145.526
  //minHipHeight indicates change of state
  //double minHipHeight = 146.1; 

  double standHeight = -1;

  // state machine
  switch(state) 
  {
    case 0: //sit
    {
      standHeight = sitHeight; // sit

      graspDistState = GraspRequest::GDS_none;
      graspStiffState = GraspRequest::GSS_none;
      if (comHeight < standHeight + 20) { 
        graspingState = GraspRequest::open;
      }

      if (comHeight < standHeight + 1) {
        state = 1;
      }
      break;
    }
    case 1: //open
    {
      standHeight = sitHeight; // sit

      graspDistState = GraspRequest::GDS_none;
      graspStiffState = GraspRequest::GSS_none;
      graspingState = GraspRequest::open;


      if (state_time > timeForSitState) {
        state = 2;
      }
      break;
    }
    case 2: //grasping
    {
      standHeight = sitHeight; // sit
      graspingState = GraspRequest::empty;
      
      if (state_time > timeForGraspState) {
        state = 3;
      }
      break;
    }
    case 3: //end
    {
      standHeight = -1;
      graspingState = GraspRequest::empty;

      Vector3d heighGraspingCenter(145, 0, 120);
      const Pose3D& chestPose = getKinematicChain().theLinks[KinematicChain::Torso].M;
      heighGraspingCenter = chestPose * heighGraspingCenter;
      getMotionRequest().graspRequest.graspingPoint = heighGraspingCenter;

      state = 3;
      break;
    }
    default:
      // do nothing
      break;
  }//end switch

  getMotionRequest().id = motion::stand;
  getMotionRequest().standHeight = standHeight; // sit in a stable position


  oldFrameNumber = getFrameInfo().getFrameNumber();

}//end take_object_from_table



void GraspingBehaviorControl::track_and_take_object()
{

  // HACK: need a solution here
  double force = 0;//getSerialSensorData().getMaxForce();
  static bool grasped = false;

  grasped = grasped?force > 1.0:force > 1.6;
  PLOT("GraspingBehaviorControl:grasped", grasped);

  static int state = -1;
  static int old_state = state;
  static double state_start_time = getFrameInfo().getTimeInSeconds(); // in s

  //to reset the state, when DEBUG-REQUEST isnt selected
  static unsigned int oldFrameNumber = getFrameInfo().getFrameNumber();
  if (getFrameInfo().getFrameNumber() - oldFrameNumber > 1)
  {
    state = -1;
  }

  // state changed
  if(state != old_state)
  {
    old_state = state;
    state_start_time = getFrameInfo().getTimeInSeconds();
  }

  // the time of the curren state
  double state_time = getFrameInfo().getTimeInSeconds() - state_start_time;

  //to set graspingState
  //GraspRequest::GraspingState& graspingState = GraspRequest::none;
  GraspRequest::GraspingState& graspingState = getMotionRequest().graspRequest.graspingState;
  GraspRequest::GraspDistState& graspDistState = getMotionRequest().graspRequest.graspDistState;
  GraspRequest::GraspStiffState& graspStiffState = getMotionRequest().graspRequest.graspStiffState;

  double timeForGraspState = 1.0; //s
  MODIFY("GraspingBehaviorControl:track_the_object:timeForGraspState", timeForGraspState);

  Vector3d defaultGraspingCenter(145, 0, 90);
  const Pose3D& chestPose = getKinematicChain().theLinks[KinematicChain::Torso].M;
  defaultGraspingCenter = chestPose * defaultGraspingCenter;
  getMotionRequest().graspRequest.graspingPoint = defaultGraspingCenter;

  static Vector3d holdingPoint = defaultGraspingCenter;

  double mod_state = state;
  MODIFY("GraspBehaviorControl:track_the_object:state", mod_state);

  //fill buffer
  Vector3d newBallPosition = getBallPercept().sizeBasedRelativePosition; //theGraspingBallModel.position;

  //if (theGraspingBallModel.valid)
  if(getBallPercept().ballWasSeen)
    ballBuffer.add(newBallPosition);

  // state machine
  switch(state)
  {
    case -1: // search
    {
      graspDistState = GraspRequest::GDS_none;
      graspStiffState = GraspRequest::GSS_none;
      graspingState = GraspRequest::open;

      getMotionRequest().graspRequest.graspingPoint = defaultGraspingCenter;
      getHeadMotionRequest().id = HeadMotionRequest::look_straight_ahead;
      
      //if(theGraspingBallModel.valid)
      if(getBallPercept().ballWasSeen)
      {
        state = 0; //search
      }
      break;
    }
    case 0: //tracking
    {
      graspDistState = GraspRequest::GDS_none;
      graspStiffState = GraspRequest::GSS_none;
      graspingState = GraspRequest::open;


      getHeadMotionRequest().id = HeadMotionRequest::look_at_world_point;
      getHeadMotionRequest().targetPointInTheWorld = getBallPercept().sizeBasedRelativePosition;//theGraspingBallModel.position;
        
      if (ballBuffer.size() >= 40) 
      {

        getMotionRequest().graspRequest.graspingPoint = getBallPercept().sizeBasedRelativePosition;//theGraspingBallModel.position;
        //getMotionRequest().graspRequest.graspingPoint = theBallPercept.sizeBasedRelativePosition;

        double threshold = 1.5;
        MODIFY("GraspBehaviorControl:track_the_object:threshold", threshold);

        //debug
        //check the longest error between mean and ballBuffer entries
        double biggestError = 0;
        for (int i = 1; i < ballBuffer.size(); i++)
        {
          //biggestError += (ballBuffer.getMean() - ballBuffer.buffer[i]).abs();
          biggestError += (ballBuffer[i-1] - ballBuffer[i]).abs();
        }

        biggestError /= ((double)ballBuffer.size());

        PLOT("GraspBehaviorControl:track_the_object:ball_motion_radius", biggestError);



        const Vector3d lHandOffset(NaoInfo::LowerArmLength+NaoInfo::HandOffsetX,0,0);
        const Vector3d rHandOffset(NaoInfo::LowerArmLength+NaoInfo::HandOffsetX,0,0);
        const Vector3d lHandPoint = getKinematicChain().theLinks[KinematicChain::LForeArm].M * lHandOffset;
        const Vector3d rHandPoint = getKinematicChain().theLinks[KinematicChain::RForeArm].M * rHandOffset;
        const Vector3d handCenter = (lHandPoint + rHandPoint)*0.5;
        double center_error = (handCenter-getMotionRequest().graspRequest.graspingPoint).abs();

        PLOT("GraspBehaviorControl:track_the_object:center_error", center_error);

        //if(!theGraspingBallModel.valid)
        if(!getBallPercept().ballWasSeen)
        {
          state = -1; //search
        }
        else if (biggestError <= threshold && center_error < 20.0)
        {
          state = 1;
        }

        break;
      }
    }

    case 1: //grasping
    {
      getHeadMotionRequest().id = HeadMotionRequest::look_at_world_point;
      getHeadMotionRequest().targetPointInTheWorld = getBallPercept().sizeBasedRelativePosition;//theGraspingBallModel.position;

      getMotionRequest().graspRequest.graspingPoint = getBallPercept().sizeBasedRelativePosition;//theGraspingBallModel.position;
      
      graspingState = GraspRequest::empty;


      if (state_time > 2.0)
      {
        if(grasped)
        {
          state = 2;
          holdingPoint = getMotionRequest().graspRequest.graspingPoint;
        }
        else
          state = 0;
      }
      break;
    }
    case 2: //hold
    {

      graspingState = GraspRequest::empty;
      getHeadMotionRequest().id = HeadMotionRequest::look_at_world_point;
      getHeadMotionRequest().targetPointInTheWorld = getBallPercept().sizeBasedRelativePosition;//theGraspingBallModel.position;
      getMotionRequest().graspRequest.graspingPoint = holdingPoint;

      if(!grasped)
        state = 0;

      break;
    }
    default:
      // do nothing
      break;
  }//end switch

  getMotionRequest().id = motion::stand;
  getMotionRequest().standHeight = -1; // sit in a stable position

  oldFrameNumber = getFrameInfo().getFrameNumber();

}//end track_and_take_object
