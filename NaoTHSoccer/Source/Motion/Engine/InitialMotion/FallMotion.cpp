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
  stiffness_increase = getRobotInfo().getBasicTimeStepInSecond() * 5;
}


void FallMotion::execute() {
    if (getMotionRequest().id != getId())
    {
        if (setStiffness(getMotorJointData(), getSensorJointData(), oldStiffness, stiffness_increase)) {
            setCurrentState(motion::stopped);
        }
        for (size_t i = 0; i < JointData::numOfJoint; i++) {
            getMotorJointData().position[i] = getSensorJointData().position[i];
        }
        return;
    }
    if (isStopped()) // runs once on start
    {
      startTime = getFrameInfo();
      for (size_t i = 0; i < JointData::numOfJoint; i++) { // store joint data
          oldStiffness[i] = getSensorJointData().stiffness[i];
//          oldJoints[i] = getSensorJointData().position[i];
      }
    }


    int t = getFrameInfo().getTimeSince(startTime); // in ms
    if(abs(getInertialSensorData().data.y) > 20 || t < 1500) { // assure motion lasts 2s max
        /*
            HeadPitch,
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
            RHand
        */
        const bool falling_forwards = Math::toDegrees(getInertialSensorData().data.y) > 0;
        const std::vector<keyFrame>& key_frames = falling_forwards ? forwards : backwards;

        for (keyFrame kf : key_frames) {
            if (t > kf.triggerMs || Math::toDegrees(getInertialSensorData().data.y) > kf.altTriggerGrad) {
                continue;
            }
            for (int i = 0; i < 22; ++i)
            {
                getMotorJointData().position[i] = Math::fromDegrees(kf.jointValues[i]);
                getMotorJointData().stiffness[i] = kf.stiffnessValues[i] / 100.0;
                std::cerr << kf.jointValues[i] << " ";
            }

            /*for (int i = 0; i < 22; ++i) {
                std::cerr << kf.stiffnessValues[i] << " ";
            }
            std::cerr << "y: " << Math::toDegrees(getInertialSensorData().data.y) << " t: " << t << "\n";*/
            break;
        }
    }
    else { // will be triggered if we are still standing or if we fall sideways
        int x[] = {0,0,0,0,90,90,0,0,-100,100,0,0,-24,-24,0,0,105,105,0,0,0,0}; // maybe bend knees less here
        for (int i = 0; i < 22; ++i) {
            getMotorJointData().position[i]  = Math::fromDegrees(x[i]);
            getMotorJointData().stiffness[i] = 0.1;
        }
    }


    setCurrentState(motion::running);
    // todo: stiffness ready hack?
    // todo: end own motion?
}
