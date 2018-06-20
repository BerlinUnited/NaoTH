#ifndef _ArmCollisionDetector2018_H_
# define _ArmCollisionDetector2018_H_

#include <ModuleFramework/Module.h>

//Representations
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Motion/MotionStatus.h>
#include <Representations/Motion/Request/MotionRequest.h>
#include <Representations/Motion/CollisionPercept.h>
//Tools
#include "Tools/DataStructures/Point.h"
#include <Tools/Math/ConvexHull.h>
#include <Tools/DataStructures/RingBufferWithSum.h>
#include <vector>
#include <string>
//Debug
#include <Tools/Debug/DebugRequest.h>
#include <Tools/Debug/DebugPlot.h>
#include <Tools/Debug/DebugModify.h>
#include <Tools/Debug/DebugParameterList.h>


BEGIN_DECLARE_MODULE(ArmCollisionDetector2018)
PROVIDE(DebugRequest)
PROVIDE(DebugPlot)
PROVIDE(DebugModify)
PROVIDE(DebugParameterList)
PROVIDE(CollisionPercept)

REQUIRE(FrameInfo)
REQUIRE(MotorJointData)
REQUIRE(SensorJointData)
REQUIRE(MotionStatus)
REQUIRE(MotionRequest)
END_DECLARE_MODULE(ArmCollisionDetector2018)

class ArmCollisionDetector2018 : private ArmCollisionDetector2018Base
{
public:
	ArmCollisionDetector2018();
	~ArmCollisionDetector2018();
	void execute();

public:
	class Parameter : public ParameterList
	{
	public:
		Parameter() : ParameterList("ArmCollisionDetector2018")
		{
			//Entweder direkt als Point vektor
			//PARAMETER_REGISTER(ReferenceHull) = vector<Point>;
			//Oder als Pfad zur txt was vermutlich einfacher ist
			PARAMETER_REGISTER(point_config) = "A:\B\C\d.txt";
			syncWithConfig();
		}
		std::string point_config;
	} params;

private:
	//Private variablen wie zb ringbuffer fuer MJD und SJD synchronisation
	RingBuffer<double, 4> jointDataBufferLeft;
	RingBuffer<double, 4> jointDataBufferRight;
	std::vector<Point> P;
};

#endif