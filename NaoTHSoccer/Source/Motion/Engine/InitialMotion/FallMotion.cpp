/**
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter, Stella Alice</a>
*/
#include "FallMotion.h"
#include "Representations/Infrastructure/InertialSensorData.h"

using namespace naoth;

FallMotion::FallMotion()
  :
  AbstractMotion(motion::falling, getMotionLock())
{
  stiffness_increase = getRobotInfo().getBasicTimeStepInSecond() * 5;
  t_since_fall_start = -1;

/*      HeadPitch,
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
  fall_times_front = {5, 120}; // in s/12

  fall_stiffness_front_map = {
  {100,100,  100,100,100,100,  100,100,100,100, 20,20,20,20,20,20, 20,20,20,20,20,20, 30, 30, 30, 30},
  { 30, 30,   15, 15, 15, 15,   15, 15, 15, 15, 20,20,20,20,20,20, 20,20,20,20,20,20, 30, 30, 30, 30},
  {  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,  0,  0}};
  fall_t_stiffness_front = {5, 3, 120};

  fall_back_map = {
  {29,0,-12,12,123,123,0,0,-17,17,0,0,-90,-90,0,0,105,105,-45,-45,0,0},
  {29,0,-12,12,123,123,78,-78,-17,17,0,0,-90,-90,0,0,105,105,-45,-45,0,0}};
  fall_times_back = {5, 120};

  fall_stiffness_back_map = {
  {100,100,  100,100,100,100,  100,100,100,100, 20,20,20,20,20,20, 20,20,20,20,20,20, 30, 30, 30, 30},
  { 30, 30,   15, 15, 15, 15,   15, 15, 15, 15, 20,20,20,20,20,20, 20,20,20,20,20,20, 30, 30, 30, 30},
  {  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,  0,  0}};
  fall_t_stiffness_back = {5, 3, 120};
}


void FallMotion::execute() {
    t_since_fall_start += 1; // it is 0 in the first run because it is initialized with -1
    if(getMotionRequest().id != getId()) { // if requested id is not our id, then be done
        if ( setStiffness(getMotorJointData(), getSensorJointData(), oldStiffness, stiffness_increase) ) { // gradually restore hardness when done
            setCurrentState(motion::stopped); // runs after code in if() stops/ is done/ reports true
        }
        for (size_t i = 0; i < JointData::numOfJoint; i++) { // copy sensor positions
            getMotorJointData().position[i] = getSensorJointData().position[i];
        }
        return;
    }
    else if( isStopped() ) {
      for (size_t i = 0; i < JointData::numOfJoint; i++) { // store hardness
          oldStiffness[i] = getSensorJointData().stiffness[i];
      }
    }


    std::vector<std::vector<int>> map;
    std::vector<std::vector<int>> stiffness_map;
    std::vector<int> fall_times;
    std::vector<int> fall_t_stiffness;
    if (Math::toDegrees(getIMUData().rotation.y) > 0) { // check at each execution
        map = fall_front_map;
        stiffness_map = fall_stiffness_front_map;
        fall_times = fall_times_front;
        fall_t_stiffness = fall_t_stiffness_front;
    } else { // backwards
        map = fall_back_map;
        stiffness_map = fall_stiffness_back_map;
        fall_times = fall_times_back;
        fall_t_stiffness = fall_t_stiffness_back;
    }


    // to set joint data
    for (int i = 0; i < size(fall_times); i++) {
        if ( t_since_fall_start < fall_times[i] ) {


//            for (int x : map[i]){
//                std::cerr << x << " ";
//            } std::cerr << "y-axis degree: " << Math::toDegrees(InertialSensorData().data.y) << "\n";


            for (int j = 0; j < map[i].size(); j++) {
                getMotorJointData().position[JointData::getJointID(j)] = Math::fromDegrees(map[i][j]);
            }
            break;
        }
    }


    // to set stiffness data todo: DRY this?
    for (int i = 0; i < size(fall_t_stiffness); i++) {
        if ( t_since_fall_start < fall_times[i] ) {
            for (int j = 0; j < stiffness_map[i].size(); j++) {
                getMotorJointData().stiffness[JointData::getJointID(j)] = stiffness_map[i][j]/100.0;
            }
            break;
        }
    }

    setCurrentState(motion::running);
    // todo: stiffness ready hack?
}
