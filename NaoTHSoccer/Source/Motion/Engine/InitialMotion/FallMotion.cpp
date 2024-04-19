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
//          oldJoints[i] = getSensorJointData().position[i];
      }
    }


    int t = getFrameInfo().getTimeSince(startTime); // in ms
    if(abs(getInertialSensorData().data.y) > 20 || t < 1500) { // assure motion lasts 2s max
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
        bool fallingForward = (Math::toDegrees(getInertialSensorData().data.y) > 0);

        std::vector<keyFrame> forwards = {
            {
                {-38,0,-10,10,90,90, 0,  0,-100,100,0,0,-24,-24,0,0,105,105,-75,-75,0,0},
                {100,100,  100,100,100,100,  100,100,100,100, 20,20,20,20,20,20, 20,20,20,20},
                100,
                30
            },{
                {-38,0,-10,10, 5, 5,60,-60,-100,100,0,0,-24,-24,0,0,105,105,-75,-75,0,0},
                {100,100,  100,100,100,100,  100,100,100,100, 20,20,20,20,20,20, 20,20,20,20},
                600,
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
                600,
                70
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

            /*for (int i = 0; i < 22; ++i) {
                std::cerr << kf.stiffnessValues[i] << " ";
            }
            std::cerr << "y: " << Math::toDegrees(getInertialSensorData().data.y) << " t: " << t << "\n";*/
            break;
        }
    }
    else { // will be triggered if we are still standing or if we fall sideways
        int x[] = {0,0,0,0,90,90,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // todo: bend knees just a touch
        for (int i = 0; i < 22; ++i) {
            getMotorJointData().position[i]  = Math::fromDegrees(x[i]);
            getMotorJointData().stiffness[i] = 0.15;
        }
    }


    setCurrentState(motion::running);
    // todo: stiffness ready hack?
    // todo: end own motion?
}
