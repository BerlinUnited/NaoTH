//ArmCollisionDetector2018
//Created by Etienne C.-C.

#include "ArmCollisionDetector2018.h"
#include <PlatformInterface/Platform.h>
using namespace naoth;

ArmCollisionDetector2018::ArmCollisionDetector2018()
{
	//Debug Requests for easy acces
	


	getDebugParameterList().add(&params);

	std::string line;

  const std::string& dirlocation = Platform::getInstance().theConfigDirectory;
  std::cout << dirlocation + params.point_config << std::endl;
  std::ifstream file(dirlocation + params.point_config);
	if (file.is_open() && file.good())
	{
		std::cout << "[ArmCollisionDetector2018] Opened configuration file" << std::endl;
		while (std::getline(file, line))
		{
			std::cout << "[ArmCollisionDetector2018] Read line:" << std::endl;
			std::string::size_type sz;
			double alpha = std::stod(line, &sz);
			double beta = std::stod(line.substr(sz));
			Point buff(alpha, beta);
			getCollisionPercept().referenceHull.push_back(buff);
		}
	}

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

	//Fill up our Point object vectors we want to check when we have enough
	//Ringbuffers may be unnecessary, do we really want to calculate Hull every frame?
	//Or only calculate every params.collect frames
	if (jointDataBufferLeft.isFull()) {
		double a = jointDataBufferLeft.first();
		double b = getSensorJointData().position[JointData::LShoulderPitch];
		double er = (a - b);
		getCollisionPercept().pointBufferLeft.push_back(Point(a, er));
	}
	if (jointDataBufferRight.isFull()) {
		double a = jointDataBufferRight.first();
		double b = getSensorJointData().position[JointData::RShoulderPitch];
		double er = (a - b);
		getCollisionPercept().pointBufferRight.push_back(Point(a, er));
	}

	if (getCollisionPercept().pointBufferLeft.size() == params.collect)
	{
		//Convex Hull calculation
		//First concatenate PointBuffer to reference points
		//Next compare result to reference points
		getCollisionPercept().pointBufferLeft.insert(getCollisionPercept().pointBufferLeft.end(), getCollisionPercept().referenceHull.begin(), getCollisionPercept().referenceHull.end());
		getCollisionPercept().newHullLeft = ConvexHull::convexHull(getCollisionPercept().pointBufferLeft);
    if (getCollisionPercept().newHullLeft == getCollisionPercept().referenceHull)
		{
			//No Collision
			getCollisionPercept().newHullLeft.erase(getCollisionPercept().newHullLeft.begin(), getCollisionPercept().newHullLeft.end());
			getCollisionPercept().pointBufferLeft.erase(getCollisionPercept().pointBufferLeft.begin(), getCollisionPercept().pointBufferLeft.end());
		}
		else
		{
			//Collision
			getCollisionPercept().timeCollisionArmLeft = getFrameInfo().getTime();

			getCollisionPercept().newHullLeft.erase(getCollisionPercept().newHullLeft.begin(), getCollisionPercept().newHullLeft.end());
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