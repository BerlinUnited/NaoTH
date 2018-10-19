//ArmCollisionDetector2018
//Created by Etienne C.-C.

#include "ArmCollisionDetector2018.h"
#include <PlatformInterface/Platform.h>
using namespace naoth;

ArmCollisionDetector2018::ArmCollisionDetector2018()
{
	//Debug Requests
	DEBUG_REQUEST_REGISTER("Motion:ArmCollisionDetector2018:showReferenceHull", "", false);
	DEBUG_REQUEST_REGISTER("Motion:ArmCollisionDetector2018:showLeftBuffer", "", false);
	DEBUG_REQUEST_REGISTER("Motion:ArmCollisionDetector2018:showRightBuffer", "", false);


	getDebugParameterList().add(&params);

	std::string line;

	const std::string& dirlocation = Platform::getInstance().theConfigDirectory;
	std::cout << dirlocation + params.point_config << std::endl;
	std::ifstream file(dirlocation + params.point_config);
	if (file.is_open() && file.good())
	{
		while (std::getline(file, line))
		{
			std::string::size_type sz;
			double alpha = std::stod(line, &sz);
			double beta = std::stod(line.substr(sz));
			Point buff(alpha, beta);
			getCollisionPercept().referenceHull.push_back(buff);
		}
	}
	std::sort(getCollisionPercept().referenceHull.begin(), getCollisionPercept().referenceHull.end());

}

ArmCollisionDetector2018::~ArmCollisionDetector2018()
{
	getDebugParameterList().remove(&params);
}

void ArmCollisionDetector2018::execute()
{
	DEBUG_REQUEST("Motion:ArmCollisionDetector2018:showReferenceHull",
		for (size_t i = 0; i <getCollisionPercept().referenceHull.size(); i++){
			PLOT_GENERIC("ArmCollisionDetector2018:referenceHull", getCollisionPercept().referenceHull[i].getX(), getCollisionPercept().referenceHull[i].getY());
		}
	);

	DEBUG_REQUEST("Motion:ArmCollisionDetector2018:showLeftBuffer",
		for (size_t i = 0; i <getCollisionPercept().pointBufferLeft.size(); i++){
			PLOT_GENERIC("ArmCollisionDetector2018:pointBufferLeft", getCollisionPercept().pointBufferLeft[i].getX(), getCollisionPercept().pointBufferLeft[i].getY());
		}
	);

	DEBUG_REQUEST("Motion:ArmCollisionDetector2018:showRightBuffer",
		for (size_t i = 0; i <getCollisionPercept().pointBufferRight.size(); i++){
			PLOT_GENERIC("ArmCollisionDetector2018:pointBufferRight", getCollisionPercept().pointBufferRight[i].getX(), getCollisionPercept().pointBufferRight[i].getY());
		}
	);

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

	//Fill up our Point object vectors we want to check when we have enough
	//Ringbuffers may be unnecessary, do we really want to calculate Hull every frame?
	//Or only calculate every params.collect frames
	if (jointDataBufferLeft.isFull()) {
		double a = jointDataBufferLeft.first();
		double b = getSensorJointData().position[JointData::LShoulderPitch];
		double er = (b - a);
		getCollisionPercept().pointBufferLeft.push_back(Point(a, er));
	}
	if (jointDataBufferRight.isFull()) {
		double a = jointDataBufferRight.first();
		double b = getSensorJointData().position[JointData::RShoulderPitch];
		double er = (b - a);
		getCollisionPercept().pointBufferRight.push_back(Point(a, er));
	}

	if (getCollisionPercept().pointBufferLeft.size() == params.collect)
	{
		//Convex Hull calculation
		//First concatenate PointBuffer to reference points
		//Next compare result to reference points
		getCollisionPercept().pointBufferLeft.insert(getCollisionPercept().pointBufferLeft.end(), getCollisionPercept().referenceHull.begin(), getCollisionPercept().referenceHull.end());
		getCollisionPercept().newHullLeft = ConvexHull::convexHull(getCollisionPercept().pointBufferLeft);
		std::sort(getCollisionPercept().newHullLeft.begin(), getCollisionPercept().newHullLeft.end());
		if (getCollisionPercept().newHullLeft == getCollisionPercept().referenceHull)
		{
			//No Collision
			//getCollisionPercept().newHullLeft.erase(getCollisionPercept().newHullLeft.begin(), getCollisionPercept().newHullLeft.end());
			getCollisionPercept().pointBufferLeft.erase(getCollisionPercept().pointBufferLeft.begin(), getCollisionPercept().pointBufferLeft.end());
		}
		else
		{
			//Collision
			getCollisionPercept().timeCollisionArmLeft = getFrameInfo().getTime();

			//getCollisionPercept().newHullLeft.erase(getCollisionPercept().newHullLeft.begin(), getCollisionPercept().newHullLeft.end());
			getCollisionPercept().pointBufferLeft.erase(getCollisionPercept().pointBufferLeft.begin(), getCollisionPercept().pointBufferLeft.end());
		}
	}
	if (getCollisionPercept().pointBufferRight.size() == params.collect)
	{
		//Convex Hull calculation
		//First concatenate PointBuffer to reference points
		//Next compare result to reference points
		getCollisionPercept().pointBufferRight.insert(getCollisionPercept().pointBufferRight.end(), getCollisionPercept().referenceHull.begin(), getCollisionPercept().referenceHull.end());
		getCollisionPercept().newHullRight = ConvexHull::convexHull(getCollisionPercept().pointBufferRight);
		std::sort(getCollisionPercept().newHullRight.begin(), getCollisionPercept().newHullRight.end());
		if (getCollisionPercept().newHullRight == getCollisionPercept().referenceHull)
		{
			//No Collision
			getCollisionPercept().newHullRight.erase(getCollisionPercept().newHullRight.begin(), getCollisionPercept().newHullRight.end());
			getCollisionPercept().pointBufferRight.erase(getCollisionPercept().pointBufferRight.begin(), getCollisionPercept().pointBufferRight.end());
		}
		else
		{
			//Collision
			getCollisionPercept().timeCollisionArmRight = getFrameInfo().getTime();

			getCollisionPercept().newHullRight.erase(getCollisionPercept().newHullRight.begin(), getCollisionPercept().newHullRight.end());
			getCollisionPercept().pointBufferRight.erase(getCollisionPercept().pointBufferRight.begin(), getCollisionPercept().pointBufferRight.end());
		}
	}

}