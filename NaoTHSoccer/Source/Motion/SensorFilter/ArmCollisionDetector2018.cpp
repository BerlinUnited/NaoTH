//ArmCollisionDetector2018
//Created by Etienne C.-C.
// coucaste@informatik.hu-berlin.de

#include "ArmCollisionDetector2018.h"
#include <PlatformInterface/Platform.h>
#include "Tools/Math/Polygon.h"

using namespace naoth;

ArmCollisionDetector2018::ArmCollisionDetector2018()
  : collisionFilterLeft(0.01, 0.1), collisionFilterRight(0.01, 0.1)
{
    //Debug Requests
    //Debug Requests end

    getDebugParameterList().add(&params);
    double alpha, beta;
    const std::string& dirlocation = Platform::getInstance().theConfigDirectory;

    std::ifstream fileLeft(dirlocation + params.point_configLeft);
    while (fileLeft >> alpha >> beta) {
        getCollisionPercept().referenceHullLeft.emplace_back(alpha, beta);
    }
    fileLeft.close();

    std::ifstream fileRight(dirlocation + params.point_configRight);
    while (fileRight >> alpha >> beta) {
        getCollisionPercept().referenceHullRight.emplace_back(alpha, beta);
    }
    fileRight.close();


    getCollisionPercept().referenceHullLeft = ConvexHull::convexHull(getCollisionPercept().referenceHullLeft);
    getCollisionPercept().referenceHullRight = ConvexHull::convexHull(getCollisionPercept().referenceHullRight);

    refpolyL.makeFromPointSet(getCollisionPercept().referenceHullLeft);
    refpolyR.makeFromPointSet(getCollisionPercept().referenceHullRight);

}

ArmCollisionDetector2018::~ArmCollisionDetector2018()
{
    getDebugParameterList().remove(&params);
}


void ArmCollisionDetector2018::execute()
{
    //Check if robot is in a suitable situation to recognize collisions#
    const bool bodymodeOK = getBodyState().fall_down_state != BodyState::upright;
    const bool armModeOK =
        getMotionRequest().armMotionRequest.id == ArmMotionRequest::arms_synchronised_with_walk ||
        getMotionRequest().armMotionRequest.id == ArmMotionRequest::arms_down;
    const bool motionModeOK = getMotionStatus().currentMotion == motion::walk || getMotionStatus().currentMotion == motion::stand;

    const bool doingKick = getMotionStatus().currentMotion == motion::walk && getMotionRequest().walkRequest.stepControl.type == WalkRequest::StepControlRequest::KICKSTEP;

    // clear the joint command history in order to not check for collision while the robot is already executing a evasive movement for example
    if (!armModeOK || !motionModeOK || !bodymodeOK || doingKick)
    {
        jointDataBufferLeft.clear();
        jointDataBufferRight.clear();

        collisionBufferLeft.clear();
        collisionBufferRight.clear();

        collisionBufferLeftRoll.clear();
        collisionBufferRightRoll.clear();
        return;
    }


    //collect Motorjoint Data and adjust timelag (Motor is 4 Frames ahead of Sensor)
    jointDataBufferLeft.add(getMotorJointData().position[JointData::LShoulderPitch]);
    jointDataBufferRight.add(getMotorJointData().position[JointData::RShoulderPitch]);

    jointDataBufferLeftRoll.add(getMotorJointData().position[JointData::LShoulderRoll]);
    jointDataBufferRightRoll.add(getMotorJointData().position[JointData::RShoulderRoll]);

    //When the robots arms are down, they are less sensible to collisions, so in that case we lower the sensitivity
    //by using the more sensitive threshold method
    if (getMotionRequest().armMotionRequest.id == ArmMotionRequest::arms_down)
    {
        if (jointDataBufferLeft.isFull()) {
            double e = jointDataBufferLeft.first() - getSensorJointData().position[JointData::LShoulderPitch];
            collisionBufferLeft.add(std::fabs(e));
        }
        if (jointDataBufferRight.isFull()) {
            double e = jointDataBufferRight.first() - getSensorJointData().position[JointData::RShoulderPitch];
            collisionBufferRight.add(std::fabs(e));
        }
        double max_error = params.maxErrorStand;

        // collision arm left
        if (collisionBufferLeft.isFull() && collisionBufferLeft.getAverage() > max_error) {
            getCollisionPercept().timeCollisionArmLeft = getFrameInfo().getTime();
            jointDataBufferLeft.clear();
            collisionBufferLeft.clear();
        }

        // collision arm right
        if (collisionBufferRight.isFull() && collisionBufferRight.getAverage() > max_error) {
            getCollisionPercept().timeCollisionArmRight = getFrameInfo().getTime();
            jointDataBufferRight.clear();
            collisionBufferRight.clear();
        }

        return;
    } //End of threshold method
    else
    {
        collisionBufferLeft.clear();
        collisionBufferRight.clear();
    }



    // Arm Roll Collision
    if (jointDataBufferLeftRoll.isFull()) {
        double e = jointDataBufferLeftRoll.first() - getSensorJointData().position[JointData::LShoulderRoll];
        collisionBufferLeftRoll.add(e);
    }
    if (jointDataBufferRightRoll.isFull()) {
        double e = jointDataBufferRightRoll.first() - getSensorJointData().position[JointData::RShoulderRoll];
        collisionBufferRightRoll.add(e);
    }
    double max_error = params.armRollError;

    // Arm Roll collision arm left
    if (collisionBufferLeftRoll.isFull() && std::fabs(collisionBufferLeftRoll.getAverage()) > max_error) {
        getCollisionPercept().timeCollisionArmLeft = getFrameInfo().getTime();
        jointDataBufferLeftRoll.clear();
        collisionBufferLeftRoll.clear();

    }

    // Arm Roll collision arm right
    if (collisionBufferRightRoll.isFull() && std::fabs(collisionBufferRightRoll.getAverage()) > max_error) {
        getCollisionPercept().timeCollisionArmRight = getFrameInfo().getTime();
        jointDataBufferRightRoll.clear();
        collisionBufferRightRoll.clear();
    }


    collisionFilterLeft.setParameter(params.hysteresis_g0, params.hysteresis_g1);
    collisionFilterRight.setParameter(params.hysteresis_g0, params.hysteresis_g1);

    //In this part we check for collision using the "Polygon method" 

    if (jointDataBufferLeft.isFull())
    {
        double a = jointDataBufferLeft.first();
        double b = getSensorJointData().position[JointData::LShoulderPitch];
        double er = (b - a);// er < 0 means collision from behind, er > 0 from front
        if (!refpolyL.isInside(Vector2d(a, er)))
        {
          bool probableCollision = er < 0;
          //collision
          if (collisionFilterLeft.update(probableCollision, 0.3, 0.7)) {
              getCollisionPercept().lastCollisionDirection = "Back";
          } else {
              getCollisionPercept().lastCollisionDirection = "Front";
          }
          getCollisionPercept().timeCollisionArmLeft = getFrameInfo().getTime();
          jointDataBufferLeft.clear();
        }
    }

    if (jointDataBufferRight.isFull())
    {
        double a = jointDataBufferRight.first();
        double b = getSensorJointData().position[JointData::RShoulderPitch];
        double er = (b - a); // er < 0 means collision from behind, er > 0 from front
        if (!refpolyR.isInside(Vector2d(a, er)))
        {
          bool probableCollision = er < 0;

          //collision
          if (collisionFilterRight.update(probableCollision, 0.3, 0.7)) {
              getCollisionPercept().lastCollisionDirection = "Back";
          } else {
              getCollisionPercept().lastCollisionDirection = "Front";
          }
          getCollisionPercept().timeCollisionArmRight = getFrameInfo().getTime();
          jointDataBufferRight.clear();
        }
    }

}