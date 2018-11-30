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

	/*
	if (fileLeft.is_open() && fileLeft.good())
	{
		while (std::getline(fileLeft, line))
		{
			std::string::size_type sz;
			double alpha = std::stod(line, &sz);
			double beta = std::stod(line.substr(sz));
			Vector2d buff(alpha, beta);
			getCollisionPercept().referenceHullLeft.push_back(buff);
		}
	}
  fileLeft.close();

	//Right point config
	std::cout << dirlocation + params.point_configRight << std::endl;
	std::ifstream fileRight(dirlocation + params.point_configRight);
	if (fileRight.is_open() && fileRight.good())
	{
		while (std::getline(fileRight, line))
		{
			std::string::size_type sz;
			double alpha = std::stod(line, &sz);
			double beta = std::stod(line.substr(sz));
			Vector2d buff(alpha, beta);
			getCollisionPercept().referenceHullRight.push_back(buff);
		}
	}
  fileRight.close();
  */
  getCollisionPercept().referenceHullLeft = ConvexHull::convexHull(getCollisionPercept().referenceHullLeft);
  getCollisionPercept().referenceHullRight = ConvexHull::convexHull(getCollisionPercept().referenceHullRight);

  /*
  for (size_t i = 0; i <getCollisionPercept().referenceHullLeft.size(); i++)
  {
	  refpolyL.add(getCollisionPercept().referenceHullLeft[i]);
  }

  for (size_t i = 0; i <getCollisionPercept().referenceHullRight.size(); i++)
  {
	  refpolyR.add(getCollisionPercept().referenceHullRight[i]);
  }
  */
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
		if (!Math::Polygon<double>::isInside(getCollisionPercept().referenceHullLeft, Vector2d(a, er)))
		{
      //collision
      getCollisionPercept().timeCollisionArmLeft = getFrameInfo().getTime();
			std::cout << "[ArmCollisionDetector2018] Collision detected!" << std::endl;
		}
	}

	if (jointDataBufferRight.isFull())
	{
		double a = jointDataBufferRight.first();
		double b = getSensorJointData().position[JointData::RShoulderPitch];
		double er = (b - a);
		if (!Math::Polygon<double>::isInside(getCollisionPercept().referenceHullRight, Vector2d(a, er)))
		{
			//collision
      getCollisionPercept().timeCollisionArmRight = getFrameInfo().getTime();
			std::cout << "[ArmCollisionDetector2018] Collision detected!" << std::endl;
		}
	}

}
/*
void ArmCollisionDetector2018::execute()
{
	DEBUG_REQUEST("Motion:ArmCollisionDetector2018:showReferenceHull",
		for (size_t i = 0; i <getCollisionPercept().referenceHullLeft.size(); i++){
			PLOT_GENERIC("ArmCollisionDetector2018:referenceHull", getCollisionPercept().referenceHullLeft[i].x, getCollisionPercept().referenceHullLeft[i].y);
		}
	);

	DEBUG_REQUEST("Motion:ArmCollisionDetector2018:showLeftBuffer",
		for (size_t i = 0; i <getCollisionPercept().pointBufferLeft.size(); i++){
			PLOT_GENERIC("ArmCollisionDetector2018:pointBufferLeft", getCollisionPercept().pointBufferLeft[i].x, getCollisionPercept().pointBufferLeft[i].y);
		}
	);

	DEBUG_REQUEST("Motion:ArmCollisionDetector2018:showRightBuffer",
		for (size_t i = 0; i <getCollisionPercept().pointBufferRight.size(); i++){
			PLOT_GENERIC("ArmCollisionDetector2018:pointBufferRight", getCollisionPercept().pointBufferRight[i].x, getCollisionPercept().pointBufferRight[i].y);
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
	if (jointDataBufferLeft.isFull()) {
		double a = jointDataBufferLeft.first();
		double b = getSensorJointData().position[JointData::LShoulderPitch];
		double er = (b - a);
		getCollisionPercept().pointBufferLeft.push_back(Vector2d(a, er));
	}
	if (jointDataBufferRight.isFull()) {
		double a = jointDataBufferRight.first();
		double b = getSensorJointData().position[JointData::RShoulderPitch];
		double er = (b - a);
		getCollisionPercept().pointBufferRight.push_back(Vector2d(a, er));
	}

	if (getCollisionPercept().pointBufferLeft.size() == params.collect)
	{
		//Convex Hull calculation
		//First concatenate PointBuffer to reference points
		//Next compare result to reference points
		for (size_t i = 0; i < getCollisionPercept().referenceHullLeft.size(); i++){
			getCollisionPercept().pointBufferLeft.push_back(getCollisionPercept().referenceHullLeft[i]);
		}

		getCollisionPercept().newHullLeft = ConvexHull::convexHull(getCollisionPercept().pointBufferLeft);
    
		if (getCollisionPercept().newHullLeft == getCollisionPercept().referenceHullLeft)
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
		for (size_t i = 0; i < getCollisionPercept().referenceHullRight.size(); i++){
			getCollisionPercept().pointBufferRight.push_back(getCollisionPercept().referenceHullRight[i]);
		}

		getCollisionPercept().newHullRight = ConvexHull::convexHull(getCollisionPercept().pointBufferRight);


		if (getCollisionPercept().newHullRight == getCollisionPercept().referenceHullRight)
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
*/