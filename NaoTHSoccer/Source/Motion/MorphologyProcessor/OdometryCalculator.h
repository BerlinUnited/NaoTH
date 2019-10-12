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
#include "Representations/Modeling/GroundContactModel.h"

// this is needed for the rotation
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/RobotInfo.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/CalibrationData.h>
#include <Representations/Modeling/IMUData.h>

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"

// tools
#include <Tools/Math/Pose3D.h>

BEGIN_DECLARE_MODULE(OdometryCalculator)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugPlot)

  REQUIRE(FrameInfo)
  REQUIRE(GroundContactModel)
  REQUIRE(KinematicChainSensor)

  // this is needed for the rotation
  REQUIRE(CalibrationData)
  REQUIRE(GyrometerData)
  REQUIRE(IMUData)
  REQUIRE(RobotInfo)

  PROVIDE(OdometryData)
END_DECLARE_MODULE(OdometryCalculator)

class OdometryCalculator : private OdometryCalculatorBase
{
public:
  OdometryCalculator();
  ~OdometryCalculator();

  void execute();

private:
  class Parameter : public ParameterList
  {
  public:
    Parameter() : ParameterList("OdometryCalculator")
    {
      PARAMETER_REGISTER(useGyroRotationOdometry) = false;
      PARAMETER_REGISTER(useIMUDataForRotationOdometry) = true;
      syncWithConfig();
    }

    bool useGyroRotationOdometry;
    bool useIMUDataForRotationOdometry;
  } parameter;

private:
    bool init;
    Pose3D lastLeftFoot, lastRightFoot, lastHip;

    // a simple rotation model
    double accumulatedGyroRotationZ;
};

#endif //_ODOMETRY_CALCULATOR_H_
