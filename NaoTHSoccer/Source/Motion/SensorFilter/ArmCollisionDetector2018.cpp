//ArmCollisionDetector2018
//Created by Etienne C.-C.
// coucaste@informatik.hu-berlin.de

#include "ArmCollisionDetector2018.h"
#include <PlatformInterface/Platform.h>
#include "Tools/Math/Polygon.h"

using namespace naoth;

ArmCollisionDetector2018::ArmCollisionDetector2018()
{
	//Debug Requests
	DEBUG_REQUEST_REGISTER("Motion:ArmCollisionDetector2018:showReferenceHull", "", false);
	DEBUG_REQUEST_REGISTER("Motion:ArmCollisionDetector2018:showLeftBuffer", "", false);
	DEBUG_REQUEST_REGISTER("Motion:ArmCollisionDetector2018:showRightBuffer", "", false);

	getDebugParameterList().add(&params);
	double alpha, beta;
	const std::string& dirlocation = Platform::getInstance().theConfigDirectory;

	std::ifstream fileLeft(dirlocation + params.point_configLeft);
	while (fileLeft>>alpha>>beta) {
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

  // test for the new make-method
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

	// clear the joint command history in order to not check for collision while the robot is already executing a evasive movement for example
	if (!armModeOK || !motionModeOK || !bodymodeOK)
	{
		jointDataBufferLeft.clear();
		jointDataBufferRight.clear();
        lastIllegalTime = getFrameInfo().getTime();
		return;
	}

    //HACK: Robot is reacting to collision during stand up motion wich leads to wrong interpretations
    /*
    if (getFrameInfo().getTimeSince(lastIllegalTime)<1)
    {
        return;
    }
    */

    //collect Motorjoint Data and adjust timelag (Motor is 4 Frames ahead of Sensor)
	jointDataBufferLeft.add(getMotorJointData().position[JointData::LShoulderPitch]);
	jointDataBufferRight.add(getMotorJointData().position[JointData::RShoulderPitch]);

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
	}
    else
    {
        collisionBufferLeft.clear();
        collisionBufferRight.clear();
    }

    //In this part we check for collision using the "Polygon method" 

	if (jointDataBufferLeft.isFull()) 
	{
		double a = jointDataBufferLeft.first();
		double b = getSensorJointData().position[JointData::LShoulderPitch];
		double er = (b - a);
		//if (!Math::Polygon<double>::isInside(getCollisionPercept().referenceHullLeft, Vector2d(a, er)))
		if (!refpolyL.isInside(Vector2d(a, er)))
		{
			//collision
			getCollisionPercept().timeCollisionArmLeft = getFrameInfo().getTime();
			jointDataBufferLeft.clear();
		}
	}

	if (jointDataBufferRight.isFull())
	{
		double a = jointDataBufferRight.first();
		double b = getSensorJointData().position[JointData::RShoulderPitch];
		double er = (b - a);
		//if (!Math::Polygon<double>::isInside(getCollisionPercept().referenceHullRight, Vector2d(a, er)))
		if (!refpolyR.isInside(Vector2d(a, er)))
		{
			//collision
			getCollisionPercept().timeCollisionArmRight = getFrameInfo().getTime();
			jointDataBufferRight.clear();
		}
	}

}