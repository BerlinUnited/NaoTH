

#ifndef _SensorLogger_H_
#define _SensorLogger_H_

#include <ModuleFramework/Module.h>
#include <Tools/Logfile/LogfileManager.h>

#include <Representations/Infrastructure/FrameInfo.h>

// sensors
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/ButtonData.h>

//
#include "Representations/Motion/MotionStatus.h"
#include <Representations/Modeling/IMUData.h>

// tools
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(SensorLogger)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  
  // sensors
  REQUIRE(SensorJointData)
  REQUIRE(FSRData)
  REQUIRE(AccelerometerData)
  REQUIRE(GyrometerData)
  REQUIRE(InertialSensorData)

  REQUIRE(ButtonData)

  //
  REQUIRE(MotorJointData)
  REQUIRE(MotionStatus)
  REQUIRE(IMUData)

END_DECLARE_MODULE(SensorLogger)

class SensorLogger : public SensorLoggerBase
{
public:
  SensorLogger();
  virtual ~SensorLogger();

  virtual void execute();

private:
  struct Parameters: public ParameterList
  {
    Parameters() : ParameterList("SensorLogger")
    {
      PARAMETER_REGISTER(record_init) = false;
      PARAMETER_REGISTER(log_path) = "/home/nao/sensor.log";
      syncWithConfig();
    }

    bool record_init;
    std::string log_path;
  } params;

private:
  LogfileManager logfileManager;
};

#endif // _SensorLogger_H_
