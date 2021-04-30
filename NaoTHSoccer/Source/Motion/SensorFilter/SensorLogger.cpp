
#include "SensorLogger.h"

using namespace std;

SensorLogger::SensorLogger()
{
  logfileManager.openFile("/dev/shm/sensor.log");
  getDebugParameterList().add(&params);
}

SensorLogger::~SensorLogger()
{
  logfileManager.closeFile();
  getDebugParameterList().remove(&params);
}

#define LOGSTUFF(name) \
  { std::ostream& dataStream = logfileManager.log(getFrameInfo().getFrameNumber(), #name); \
  Serializer<name>::serialize(get##name(), dataStream); } ((void)0)

void SensorLogger::execute()
{
  // HACK: wait a bit before starting recording
  if(!logfileManager.is_ready()) {
    return;
  }

  // only record in init if explicitly required
  if(params.record_init || getMotionStatus().currentMotion != motion::init)
  {
    LOGSTUFF(FrameInfo);

    // sensors
    LOGSTUFF(SensorJointData);
    LOGSTUFF(FSRData);
    LOGSTUFF(AccelerometerData);
    LOGSTUFF(GyrometerData);
    LOGSTUFF(InertialSensorData);

    LOGSTUFF(ButtonData);

    //
    LOGSTUFF(MotorJointData);
    LOGSTUFF(MotionStatus);
    LOGSTUFF(IMUData);
  }
}

