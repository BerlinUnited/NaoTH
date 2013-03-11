/**
* @file OdometryCalculator.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* OdometryCalculator: calculate the odometry according to morphology and support foot
*/

#ifndef _ODOMETRY_CALCULATOR_H_
#define _ODOMETRY_CALCULATOR_H_

#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/FSRData.h>
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Modeling/KinematicChain.h"

// tools
#include <Tools/Math/Pose3D.h>

BEGIN_DECLARE_MODULE(OdometryCalculator)
  REQUIRE(FSRData)
  REQUIRE(KinematicChainSensor)

  PROVIDE(OdometryData)
END_DECLARE_MODULE(OdometryCalculator)

class OdometryCalculator : private OdometryCalculatorBase
{
public:
  OdometryCalculator();
  ~OdometryCalculator();

  void execute();

private:
    bool supportFoot; // TRUE: left foot; FALSE: right foot;
    bool init;
    Pose3D lastLeftFoot, lastRightFoot, lastHip;
};

#endif //_ODOMETRY_CALCULATOR_H_
