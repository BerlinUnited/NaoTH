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


void FallMotion::execute() 
{
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
      // decide on the falling direction once
      // TODO: maybe the decision should be made by a separate module and passed here
      fallingForward = (Math::toDegrees(getInertialSensorData().data.y) > 0);
      startTime = getFrameInfo();

      // store old joint data and stiffness
      for (size_t i = 0; i < JointData::numOfJoint; i++) {
          oldStiffness[i] = getSensorJointData().stiffness[i];
//          oldJoints[i] = getSensorJointData().position[i];
      }
    }

    // time since start in ms
    const int t = getFrameInfo().getTimeSince(startTime); 

    if(t < 1500) // assure motion lasts 2s max
    { 
        const std::vector<keyFrame>& currKf = fallingForward ? forwards : backwards;

        double t_kf_end = 0;
        for (const keyFrame& kf : currKf)
        {
            // time when the current kf will end
            t_kf_end += kf.triggerMs;

            if (t > t_kf_end || Math::toDegrees(getInertialSensorData().data.y) > kf.altTriggerGrad) {
                continue;
            }

            for (int i = 0; i < 22; ++i)
            {
                getMotorJointData().position[i]  = Math::fromDegrees(kf.jointValues[i]);
                getMotorJointData().stiffness[i] = kf.stiffnessValues[i] / 100.0;
                //std::cerr << kf.jointValues[i] << " ";
            }

            /*for (int i = 0; i < 22; ++i) {
                std::cerr << kf.stiffnessValues[i] << " ";
            }
            std::cerr << "y: " << Math::toDegrees(getInertialSensorData().data.y) << " t: " << t << "\n";*/
            break;
        }
    }
    else 
    { 
        // TODO: is will also be triggered after the end of the falling motion. Why?
        // will be triggered if we are still standing or if we fall sideways
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
