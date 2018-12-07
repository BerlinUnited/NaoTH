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
	//Check if robot is in a suitable situation to recognize collisions
	const bool armModeOK =
		getMotionRequest().armMotionRequest.id == ArmMotionRequest::arms_synchronised_with_walk ||
		getMotionRequest().armMotionRequest.id == ArmMotionRequest::arms_down;

	const bool motionModeOK = getMotionStatus().currentMotion == motion::walk || getMotionStatus().currentMotion == motion::stand;

	if (!armModeOK || !motionModeOK)
	{
		// clear the joint command history
		jointDataBufferLeft.clear();
		jointDataBufferRight.clear();

		return;
	}

  //collect Motorjoint Data and adjust timelag (Motor is 4 Frames ahead of Sensor)
	jointDataBufferLeft.add(getMotorJointData().position[JointData::LShoulderPitch]);
	jointDataBufferRight.add(getMotorJointData().position[JointData::RShoulderPitch]);

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
		}
	}

}