/**
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter, Stella Alice</a>
*/
#include <array>
#include "FallMotion.h"

using namespace naoth;

FallMotion::FallMotion()
  :
  AbstractMotion(motion::falling, getMotionLock())
{
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
/*  fall_front_map = {
  {-38,0,-10,10,90,90, 0,  0,-100,100,0,0,-24,-24,0,0,105,105,-75,-75,0,0},
  {-38,0,-10,10, 5, 5,60,-60,-100,100,0,0,-24,-24,0,0,105,105,-75,-75,0,0},
  {-38,0,-10,10, 5, 5,60,-60,-100,100,0,0,-24,-24,0,0,105,105,-75,-75,0,0}};
  fall_times_front = {400, 600, 2000};
  ASSERT_MSG(size(fall_front_map) == size(fall_times_front), "Number of times must match map")

  fall_stiffness_front_map = {
  {100,100,  100,100,100,100,  100,100,100,100, 20,20,20,20,20,20, 20,20,20,20,20,20, 30, 30, 30, 30},
  { 30, 30,   15, 15, 15, 15,   15, 15, 15, 15, 20,20,20,20,20,20, 20,20,20,20,20,20, 30, 30, 30, 30},
  {  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,  0,  0}};
  fall_t_stiffness_front = {400, 600, 2000};

  fall_back_map = {
  {29,0,-12,12,123,123, 0,  0,-17,17,0,0,-90,-90,0,0,105,105,-45,-45,0,0},
  {29,0,-12,12,123,123,78,-78,-17,17,0,0,-90,-90,0,0,105,105,-45,-45,0,0},
  {29,0,-12,12,123,123,78,-78,-17,17,0,0,-90,-90,0,0,105,105,-45,-45,0,0}};
  fall_times_back = {400, 600, 2000};

  fall_stiffness_back_map = {
  {100,100,  100,100,100,100,  100,100,100,100, 20,20,20,20,20,20, 20,20,20,20,20,20, 30, 30, 30, 30},
  { 30, 30,   15, 15, 15, 15,   15, 15, 15, 15, 20,20,20,20,20,20, 20,20,20,20,20,20, 30, 30, 30, 30},
  {  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,  0,  0}};
  fall_t_stiffness_back = {400, 600, 2000};*/
stiffness_increase = 0; // todo: this right?
}


void FallMotion::execute() {
    if ( getMotionRequest().id != getId() )
    {
        if ( setStiffness(getMotorJointData(), getSensorJointData(), oldStiffness, stiffness_increase) ) {
            setCurrentState(motion::stopped);
        }
        for (size_t i = 0; i < JointData::numOfJoint; i++) {
            getMotorJointData().position[i] = getSensorJointData().position[i];
        }
        return;
    }
    else if( isStopped() ) // runs once on start
    {
      startTime = getFrameInfo();
      for (size_t i = 0; i < JointData::numOfJoint; i++) { // store joint data
          oldStiffness[i] = getSensorJointData().stiffness[i];
          oldJoints[i] = getSensorJointData().position[i];
      }
    }


    int t = getFrameInfo().getTimeSince(startTime); // in ms
    if(abs(getInertialSensorData().data.y) > 20 || t < 2000) { // assure motion lasts 2s max todo: && or || or what

        bool fallingForward = (Math::toDegrees(getInertialSensorData().data.y) > 0);

        std::vector<keyFrame> forwards = {
            {
                {-38,0,-10,10,90,90, 0,  0,-100,100,0,0,-24,-24,0,0,105,105,-75,-75,0,0},
                {100,100,  100,100,100,100,  100,100,100,100, 20,20,20,20,20,20, 20,20,20,20},
                300,
                40
            },{
                {-38,0,-10,10, 5, 5,60,-60,-100,100,0,0,-24,-24,0,0,105,105,-75,-75,0,0},
                {100,100,  100,100,100,100,  100,100,100,100, 20,20,20,20,20,20, 20,20,20,20},
                700,
                70
            },{
                {-38,0,-10,10, 5, 5,60,-60,-100,100,0,0,-24,-24,0,0,105,105,-75,-75,0,0},
                {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5},
                1500,
                999
            }
        };

        std::vector<keyFrame> backwards = {
            {
                {29,0,-12,12,123,123, 0,  0,-17,17,0,0,-90,-90,0,0,105,105,-45,-45,0,0},
                {100,100,  100,100,100,100,  100,100,100,100, 20,20,20,20,20,20, 20,20,20,20},
                150,
                28
            },{
                {29,0,-12,12,123,123,78,-78,-17,17,0,0,-90,-90,0,0,105,105,-45,-45,0,0},
                {100,100,  100,100,100,100,  100,100,100,100, 20,20,20,20,20,20, 20,20,20,20},
                500,
                65
            },{
                {29,0,-12,12,123,123,78,-78,-17,17,0,0,-90,-90,0,0,105,105,-45,-45,0,0},
                {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5},
                1500,
                999
            }
        };

        std::vector<keyFrame>& currKf = fallingForward ? forwards : backwards;

        for (keyFrame kf : currKf) {
            if (t > kf.triggerMs || Math::toDegrees(getInertialSensorData().data.y) > kf.altTriggerGrad) {
                continue;
            }
            for (int i = 0; i < 22; ++i)
            {
                getMotorJointData().position[i] = Math::fromDegrees(kf.jointValues[i]);
                getMotorJointData().stiffness[i] = kf.stiffnessValues[i] / 100.0;
                std::cerr << kf.jointValues[i] << " ";
            }

            std::cerr << "yAxis: " << Math::toDegrees(getInertialSensorData().data.y) << " t: " << t << "\n";
            break;
        }
/*        for (int i = 0; i < size(fall_times); i++) { // todo: only for debug
        for (int i = 0; i < 3; i++) {
            if (t < fall_times[i]) {
                for (int j = 0; j < map[i].size(); j++) {
                for (int j = 0; j < 3; j++) { // todo: only for debug
                    getMotorJointData().position[j] = Math::fromDegrees(map[i][j]);
                    getMotorJointData().position[j]  = getSensorJointData().position[j];

                    double previous_t                = (i > 0) ? fall_times[i - 1] : 0.0;
                    std::vector<double> previous_map = (i > 0) ? map[i - 1]        : oldJoints;

                    getMotorJointData().position[j] =
                        ((t - previous_t) / ((fall_times[i] - previous_t) + 0.1)) * // yields value between 0 and 1 todo: remove "+0.1"
                        (previous_map[j] - map[i][j]) +
                        previous_map[j];
                }
                break;
            }
        }

        for (int i = 0; i < size(fall_t_stiffness); i++) { // todo: DRY this // todo: only for debug
        for (int i = 0; i < 3; i++) {
            if (t < fall_t_stiffness[i]) {
                for (int j = 0; j < stiffness_map[i].size(); j++) {
                for (int j = 0; j < 3; j++) { // todo: only for debug
                    getMotorJointData().stiffness[j] = stiffness_map[i][j] / 100.0;
                    double previous_fall_t                = (i > 0) ? fall_t_stiffness[i - 1] : 0;
                    std::vector<double> previous_fall_map = (i > 0) ? stiffness_map[i - 1]    : ;

                    getMotorJointData().position[j] =
                        ((t - previous_fall_t) / (fall_t_stiffness[i] - previous_fall_t)) * // yields value between 0 and 1
                        (previous_fall_map[j] - map[i][j]) +
                        previous_fall_map[j];
                }
                break;
            }
        }*/
    } else {
        int x[] = {0,0,90,90,90,90,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // todo: set arms correctly
        for (int i = 0; i < 22; ++i) {
            getMotorJointData().position[i]  = x[i];
            getMotorJointData().stiffness[i] = 0.15;
        }
    }

    setCurrentState(motion::running);
    // todo: stiffness ready hack?
}
