/**
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter, Stella Alice</a>
*/
#include <array>
#include "FallMotion.h"

using namespace naoth;

FallMotion::FallMotion()
    :
    AbstractMotion(motion::falling, getMotionLock()) {
}


void FallMotion::execute() {

    // save old stiffness
    if (isStopped()) { // runs once on start because motion::stopped is the default state of any motion
        startTime = getFrameInfo();
        for (size_t i = 0; i < JointData::numOfJoint; i++) {
            oldStiffness[i] = getSensorJointData().stiffness[i];
        }
        setCurrentState(motion::running);
    }

    // the code to stop our motion
    if (getMotionRequest().id != getId()) { // TODO: HOW TO END OWN MOTION?
        for (size_t i = 0; i < JointData::numOfJoint; i++) {
            getMotorJointData().position[i] = getSensorJointData().position[i];
        }
        setStiffness(getMotorJointData(), getSensorJointData(), oldStiffness, std::numeric_limits<double>::max());
        setCurrentState(motion::stopped);
        return;
    }

    // check how badly we are out of balance...
    if (abs(getInertialSensorData().data.y) < 20) {
        // ... if not by much, then this code is run
        constexpr int x[22] = {0, 0, 0, 0, 90, 90, 0, 0, -100, 100, 0, 0, -24, -24, 0, 0, 105, 105, 0, 0, 0, 0}; // maybe bend knees less here
        for (int i = 0; i < 22; ++i) {
            getMotorJointData().position[i]  = Math::fromDegrees(x[i]);
            getMotorJointData().stiffness[i] = 0.3;
        }
        return;
    }

    // decide if we are falling forewards or backwards
    const bool falling_forwards = Math::toDegrees(getInertialSensorData().data.y) > 0;
    const std::vector<keyFrame>& key_frames = falling_forwards ? forwards : backwards;

    // time that we spend in this motion in ms
    const int t = getFrameInfo().getTimeSince(startTime);

    // iterate over every keyframe ...
    for (keyFrame kf : key_frames) {

        // ... if we are done with the current keyframe, go to the next one in the iteration ...
        if (t > kf.triggerMs || Math::toDegrees(getInertialSensorData().data.y) > kf.altTriggerDegrees) {
            continue;
        }

        // ... else send this keyframe to the robot ...
        for (int i = 0; i < 22; ++i) {
            getMotorJointData().position[i] = Math::fromDegrees(kf.jointValues[i]);
            getMotorJointData().stiffness[i] = kf.stiffnessValues[i] / 100.0;
        }

        // ... and end the motion.
        break;
    }

    // todo: smth smth stiffness ready hack? could the start of our motion be delayed by the engine?
}
