/**
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter, Stella Alice</a>
*/
#include "FallMotion.h"

using namespace naoth;

FallMotion::FallMotion()
  :
  AbstractMotion(motion::falling, getMotionLock())
{
  //stiffness_increase = getRobotInfo().getBasicTimeStepInSecond() * 5;
  t_since_fall_start = -1;

/*    HeadPitch,
      HeadYaw,

      RShoulderRoll,
      LShoulderRoll,
      RShoulderPitch,
      LShoulderPitch,

      RElbowRoll,
      LElbowRoll,
      RElbowYaw,
      LElbowYaw,

      RHipYawPitch, // doesn't exist on Nao
      LHipYawPitch,
      RHipPitch,
      LHipPitch,
      RHipRoll,
      LHipRoll,
      RKneePitch,
      LKneePitch,
      RAnklePitch,
      LAnklePitch,
      RAnkleRoll,
      LAnkleRoll,

      LWristYaw,
      RWristYaw,
      LHand,
      RHand*/

  fall_front_map = {
  {-38,0,-10,10,90,90,0,0,-100,100,0,0,-24,-24,0,0,105,105,-75,-75,0,0},
  {-38,0,-10,10,5,5,60,-60,-100,100,0,0,-24,-24,0,0,105,105,-75,-75,0,0}};
  fall_times_front = {200, 1000}; // in ms

  fall_stiffness_front_map = {
  {100,100,  100,100,100,100,  100,100,100,100, 20,20,20,20,20,20, 20,20,20,20,20,20, 30, 30, 30, 30},
  { 30, 30,   15, 15, 15, 15,   15, 15, 15, 15, 20,20,20,20,20,20, 20,20,20,20,20,20, 30, 30, 30, 30},
  //{  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,  0,  0}
  };
  fall_t_stiffness_front = {200, 1000, /*255*/}; // in ms

  fall_back_map = {
  {29,0,-12,12,123,123,0,0,-17,17,0,0,-90,-90,0,0,105,105,-45,-45,0,0},
  {29,0,-12,12,123,123,78,-78,-17,17,0,0,-90,-90,0,0,105,105,-45,-45,0,0}};
  fall_times_back = {200, 1000}; // in ms

  fall_stiffness_back_map = {
  {100,100,  100,100,100,100,  100,100,100,100, 20,20,20,20,20,20, 20,20,20,20,20,20, 30, 30, 30, 30},
  { 30, 30,   15, 15, 15, 15,   15, 15, 15, 15, 20,20,20,20,20,20, 20,20,20,20,20,20, 30, 30, 30, 30},
  //{  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,  0,  0}
  };
  fall_t_stiffness_back = {200, 1000, /*255*/}; // in ms
}


void FallMotion::execute() 
{
    t_since_fall_start += 1; // it is 0 in the first run because it is initialized with -1
    
    // if requested id is not our id, then be done
    if(getMotionRequest().id != getId()) 
    { 
        if ( setStiffness(getMotorJointData(), getSensorJointData(), oldStiffness, stiffness_increase) ) { // gradually restore hardness when done
            setCurrentState(motion::stopped); // runs after code in if() stops/ is done/ reports true
        }
        for (size_t i = 0; i < JointData::numOfJoint; i++) { // copy sensor positions
            getMotorJointData().position[i] = getSensorJointData().position[i];
        }
        return;
    }
    else if( isStopped() ) // first time when the motion is executed
    {
      startTime = getFrameInfo();
      for (size_t i = 0; i < JointData::numOfJoint; i++) { // store hardness
          oldStiffness[i] = getSensorJointData().stiffness[i];
      }
    }

    // running time of the motion in ms
    int t = getFrameInfo().getTimeSince(startTime);

    // decide weather we are falling backwards or forward
    bool fallingForward = (Math::toDegrees(getInertialSensorData().data.y) > 0);

    std::vector<std::vector<double>>& map           = fallingForward ? fall_front_map          : fall_back_map;
    std::vector<std::vector<double>>& stiffness_map = fallingForward ? fall_stiffness_front_map: fall_stiffness_back_map;
    std::vector<double>& fall_times                 = fallingForward ? fall_times_front        : fall_times_back;
    std::vector<double>& fall_t_stiffness           = fallingForward ? fall_t_stiffness_front  : fall_t_stiffness_back;

    /*
    if (Math::toDegrees(getInertialSensorData().data.y) > 0) { // check at each execution
        map = &fall_front_map;
        stiffness_map = fall_stiffness_front_map;
        fall_times = fall_times_front;
        fall_t_stiffness = fall_t_stiffness_front;
    } else { // backwards
        map = fall_back_map;
        stiffness_map = fall_stiffness_back_map;
        fall_times = fall_times_back;
        fall_t_stiffness = fall_t_stiffness_back;
    }
    */

    // set default values for joints and stifness
    for (size_t i = 0; i < JointData::numOfJoint; i++) { // copy sensor positions
      getMotorJointData().position[i]  = getSensorJointData().position[i];
      getMotorJointData().stiffness[i] = 0;
    }

    // to set joint data
    for (int i = 0; i < size(fall_times); i++) {
        //if (abs(Math::toDegrees(getInertialSensorData().data.y)) < fall_times[i]) {
        if(t < fall_times[i]) 
        {
            //for (int x : map[i]){
            //    std::cerr << x << " ";
            //} std::cerr << "y-axis degree: " << Math::toDegrees(getInertialSensorData().data.y) << "\n";

            for (int j = 0; j < map[i].size(); j++) {
                getMotorJointData().position[JointData::getJointID(j)] = Math::fromDegrees(map[i][j]);
            }
            break;
        }
    }

    // to set stiffness data todo: DRY this?
    for (int i = 0; i < size(fall_t_stiffness); i++) {
        //if ( abs(Math::toDegrees(getInertialSensorData().data.y)) < fall_times[i]) {
        if(t < fall_times[i]) 
        {
            for (int j = 0; j < stiffness_map[i].size(); j++) {
                getMotorJointData().stiffness[JointData::getJointID(j)] = stiffness_map[i][j]/100.0;
            }
            break;
        }
    }

    setCurrentState(motion::running);
    // todo: stiffness ready hack?
}
