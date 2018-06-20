//ArmCollisionDetector2018
//Created by Etienne C.-C.

#include "ArmCollisionDetector2018.h"

ArmCollisionDetector2018::ArmCollisionDetector2018()
{
	getDebugParameterList().add(&params);
}

ArmCollisionDetector2018::~ArmCollisionDetector2018()
{
	getDebugParameterList().remove(&params);
}

void ArmCollisionDetector2018::execute()
{

}