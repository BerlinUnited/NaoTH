//ArmCollisionDetector2018
//Created by Etienne C.-C.

#include "ArmCollisionDetector2018.h"
using namespace naoth;

ArmCollisionDetector2018::ArmCollisionDetector2018()
{
	//Debug Requests for easy acces
	DEBUG_REQUEST_REGISTER("Motion:SensorFilter:Arm_and_Motion_ok", "Display if current motion status and Arm mode is valid or not", true);
	DEBUG_REQUEST_REGISTER("Motion:SensorFilter:ReferenceHull", "Print reference hull", true);
	DEBUG_REQUEST_REGISTER("Motion:SensorFilter:BufferL", "Print left point buffer", true);
	DEBUG_REQUEST_REGISTER("Motion:SensorFilter:BufferR", "Print right point buffer", true);
	DEBUG_REQUEST_REGISTER("Motion:SensorFilter:BufferL_hull", "Print calculated convex hull for left joint", true);
	DEBUG_REQUEST_REGISTER("Motion:SensorFilter:BufferR_hull", "Print calculated convex hull for left joint", true);


	getDebugParameterList().add(&params);
	std::string line;
	std::ifstream file("C:/Users/Etienne Couque/PycharmProjects/NAOpy/textnewhull.txt");
	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			std::string::size_type sz;
			double alpha = std::stod(line, &sz);
			double beta = std::stod(line.substr(sz));
			Point buff(alpha, beta);
			P.push_back(buff);
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
	DEBUG_REQUEST("Motion:SensorFilter:Arm_and:Motion_Ok", std::cout << "Arm Mode: " << armModeOK << std::endl << "Motion Mode: " << motionModeOK << std::endl;);

	if (!armModeOK || !motionModeOK)
	{
		// clear the joint command history
		jointDataBufferLeft.clear();
		jointDataBufferRight.clear();

		return;
	}

	//collect Sensor & Motorjoint Data and adjust timelag (Motor is 4 Frames ahead of Sensor)
	jointDataBufferLeft.add(getMotorJointData().position[JointData::LShoulderPitch]);
	jointDataBufferRight.add(getMotorJointData().position[JointData::RShoulderPitch]);

	//Fill up our Point object ringbuffers we want to check when we have enough
	//Ringbuffers may be unnecessary, do we really want to calculate Hull every frame?
	//Or only calculate every 15 frames
	if (jointDataBufferLeft.isFull()) {
		double a = jointDataBufferLeft.first();
		double b = getSensorJointData().position[JointData::LShoulderPitch];
		double er = (a - b);
		PointBufferLeft.push_back(Point(a, er));
	}
	if (jointDataBufferRight.isFull()) {
		double a = jointDataBufferRight.first();
		double b = getSensorJointData().position[JointData::RShoulderPitch];
		double er = (a - b);
		PointBufferRight.push_back(Point(a, er));
	}

	if (PointBufferLeft.size() == 16)
	{
		//Convex Hull calculation
		//First concatenate PointBuffer to reference points
		//Next compare result to reference points
		PointBufferLeft.insert(PointBufferLeft.end(), P.begin(), P.end());
		vBuff = ConvexHull::convexHull(PointBufferLeft);
		if (vBuff == P)
		{
			//No Collision
			vBuff.erase(vBuff.begin(), vBuff.end());
			PointBufferLeft.erase(PointBufferLeft.begin(), PointBufferLeft.end());
		}
		else
		{
			//Collision
			vBuff.erase(vBuff.begin(), vBuff.end());
			PointBufferLeft.erase(PointBufferLeft.begin(), PointBufferLeft.end());
		}
	}
	if (PointBufferRight.size() == 16)
	{
		//Convex Hull claculation
		//First concatenate PointBuffer to reference points
		//Next compare result to reference points
		PointBufferRight.insert(PointBufferRight.end(), P.begin(), P.end());
		vBuff = ConvexHull::convexHull(PointBufferRight);
		if (vBuff == P)
		{
			//No Collision
			vBuff.erase(vBuff.begin(), vBuff.end());
			PointBufferRight.erase(PointBufferRight.begin(), PointBufferRight.end());
		}
		else
		{
			//Collision
			vBuff.erase(vBuff.begin(), vBuff.end());
			PointBufferRight.erase(PointBufferRight.begin(), PointBufferRight.end());
		}
	}


}