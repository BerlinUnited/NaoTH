/**
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter, Stella Alice</a>
*/
#include "FallMotion.h"
//#include <vector>

using namespace naoth;

FallMotion::FallMotion()
  :
  AbstractMotion(motion::falling, getMotionLock())
{
  stiffness_increase = getRobotInfo().getBasicTimeStepInSecond() * 5;
  t_since_fall_start = -1;

  fall_front_map = {std::vector<int> { 40, 60, 90 }, { // the first keyframe is trying to be reached until the robot is at 40 degrees
  {0, {0.0,-38.0,90.0,10.0,100.0,0.0,-90.0,0.0,90.0,-10.0,-100.0,0.0,90.0,0.0,0.0,0.0,-24.0,105.0,-75.0,0.0,0.0,0.0,-24.0,105.0,-75.0,0.0}},
  {1, {0.0,-38.0,5.0,10.0,100.0,-60.0,-90.0,0.0,5.0,-10.0,-100.0,60.0,90.0,0.0,0.0,0.0,-24.0,105.0,-75.0,0.0,0.0,0.0,-24.0,105.0,-75.0,0.0}}, // todo: change line to be like line 1?
  {2, {0.0,-38.0,5.0,10.0,100.0,-60.0,-90.0,0.0,5.0,-10.0,-100.0,60.0,90.0,0.0,0.0,0.0,-24.0,105.0,-75.0,0.0,0.0,0.0,-24.0,105.0,-75.0,0.0}}}};

  fall_back_map = {std::vector<int> { 40, 60, 90 }, {
  {0, {0.0,29.0,123.0,12.0,17.0,  0.0,-50.0,0.0,123.0,-12.0,-17.0, 0.0,50.0,0.0,0.0,0.0,-90.0,105.0,-45.0,0.0,0.0,0.0,-90.0,105.0,-45.0,0.0}},
  {1, {0.0,29.0,123.0,12.0,17.0,-78.0,-50.0,0.0,123.0,-12.0,-17.0,78.0,50.0,0.0,0.0,0.0,-90.0,105.0,-45.0,0.0,0.0,0.0,-90.0,105.0,-45.0,0.0}},
  {2, {0.0,29.0,123.0,12.0,17.0,-78.0,-50.0,0.0,123.0,-12.0,-17.0,78.0,50.0,0.0,0.0,0.0,-90.0,105.0,-45.0,0.0,0.0,0.0,-90.0,105.0,-45.0,0.0}}}};

  fall_stiffness_front_map = {std::vector<int> { 40, 60, 90 }, { // the first keyframe is trying to be reached until the robot is at 40 degrees
  {0, {100,100,  100,100,100,100,  100,100,100,100, 20,20,20,20,20,20, 20,20,20,20,20,20,  30, 30, 30, 30}},
  {1, { 30, 30,   15, 15, 15, 15,   15, 15, 15, 15, 20,20,20,20,20,20, 20,20,20,20,20,20,  30, 30, 30, 30}}, // todo: change line to be like line 1?
  {2, { 30, 30,   15, 15, 15, 15,   15, 15, 15, 15, 20,20,20,20,20,20, 20,20,20,20,20,20,  30, 30, 30, 30}}}};

  fall_stiffness_back_map = {std::vector<int> { 40, 60, 90 }, {
  {0, {100,100,  100,100,100,100,  100,100,100,100, 20,20,20,20,20,20, 20,20,20,20,20,20,  30, 30, 30, 30}},
  {1, {100,100,  100,100,100,100,  100,100,100,100, 20,20,20,20,20,20, 20,20,20,20,20,20,  30, 30, 30, 30}},
  {2, { 30, 30,   15, 15, 15, 15,   15, 15, 15, 15, 20,20,20,20,20,20, 20,20,20,20,20,20,  30, 30, 30, 30}}}};

  assert(size(std::get<0>(fall_front_map)) == size(std::get<1>(fall_front_map)));
  assert(size(std::get<0>(fall_back_map)) == size(std::get<1>(fall_back_map)));
  assert(size(std::get<0>(fall_stiffness_front_map)) == size(std::get<1>(fall_stiffness_front_map)));
  assert(size(std::get<0>(fall_stiffness_back_map)) == size(std::get<1>(fall_stiffness_back_map)));
}

void FallMotion::execute() {
    t_since_fall_start += 1; // it is 0 in the first run because it is initialized with -1

    if(getMotionRequest().id != getId()) { // if requested id is not our id, then be done
        // gradually restore hardness when done
        if ( setStiffness(getMotorJointData(), getSensorJointData(), oldStiffness, stiffness_increase) ) {
            // after code in if() stops/ is done/ reports true:
            setCurrentState(motion::stopped);
        }

        for (size_t i = 0; i < JointData::numOfJoint; i++) { // copy sensor positions
            getMotorJointData().position[i] = getSensorJointData().position[i];
        }
        return;
    }

    else if( isStopped() ) { // executed (only?) the first time
      for (size_t i = 0; i < JointData::numOfJoint; i++) { // store hardness
          oldStiffness[i] = getSensorJointData().stiffness[i];
      }
    }

    // todo: adjust fall down values in XABSL
    //  else if(abs(value=sensor.inertial.y)>25 || abs(value=sensor.inertial.x)>20)

    // todo: stiffness ready hack?

    std::tuple<std::vector<int>, std::map<int, std::vector<double>>> map;
    std::tuple<std::vector<int>, std::map<int, std::vector<double>>> stiffness_map;
    if (getIMUData().rotation.y > Math::fromDegrees(10)){ // backwards
      map = fall_back_map;
      stiffness_map = fall_stiffness_back_map;
    }
    else if (getIMUData().rotation.y < Math::fromDegrees(-10)){ // forwards
      map = fall_front_map;
      stiffness_map = fall_stiffness_front_map;
    }


    if (abs(getIMUData().rotation.y) > 10) {
      // to set joint data:
      for (int i = 0; i < size(std::get<0>(map)); i++) { // runs once for every degree in the array of the tuple
          if (abs(getIMUData().rotation.y) <= abs(std::get<0>(map)[i])) {
              for (double j : std::get<1>(map)[i]) {
                  getMotorJointData().position[JointData::getJointID(i)] = Math::fromDegrees(j);
              }
              break;
          }
      }
      // to set stiffness data:
      for (int i = 0; i < size(std::get<0>(stiffness_map)); i++) { // runs once for every degree in the array of the tuple
          if (abs(getIMUData().rotation.y) <= abs(std::get<0>(stiffness_map)[i])) {
              for (double j : std::get<1>(stiffness_map)[i]) {
                  freeStiffness[JointData::getJointID(i)] = j;
              }
              break;
          }
      }
    }

//    setStiffness(getMotorJointData(), getSensorJointData(), freeStiffness, 10); // set joint free todo: what is this? was commented in
    setCurrentState(motion::running);
} //end execute
