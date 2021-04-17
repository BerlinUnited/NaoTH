

#ifndef _LolaAdaptor_h_
#define _LolaAdaptor_h_

// data representations
#include "Tools/LolaData.h"
#include "Tools/LolaDataConverter.h"
#include "Tools/DCMData.h"

// lola unix socket client
#include "Tools/Lola.h"

#include <Tools/ThreadUtil.h>
#include "Tools/SharedMemoryIO.h"
#include "Tools/BasicMotion.h"

#include "Tools/NaoTime.h"

#include <chrono>
#include <thread>
#include <unistd.h>

namespace naoth 
{

class LolaAdaptor
{
public:
  LolaAdaptor();
  ~LolaAdaptor();

  void start();
  void stop();

  bool isRunning() const;
private:
  /**
   * @brief Prints and opens the passed shared memory path.
   */
  template<typename T>
  void openSharedMemory(SharedMemory<T> &sm, const std::string &path);

  /**
   * @brief Writes the nao bodyId and the nickname to the "Config/nao.info" file.
   */
  void writeNaoInfo() const;

  static bool fileExists (const std::string& filename);

  void run();
  
  void setWarningLED(ActuatorData& actuators, bool red);

  bool runEmergencyMotion(ActuatorData& actuators);

  void notify();

  void shutdownCallback();

private:
  bool running;
  bool exiting;
  bool waitingForLola;
  std::thread lolaThread;

  // these things are necessary to handle shutdown
  ButtonData theButtonData;
  bool shutdown_requested;
  std::thread shutdownCallbackThread;

  enum State
  {
    DISCONNECTED,
    CONNECTED
  } state;

  // sitdown motion in case the Controller dies
  bool command_data_available;
  bool sensor_data_available;
  MotorJointData theMotorJointData;
  InertialSensorData theInertialSensorData;
  BasicMotion* initialMotion;

  // DCM --> NaoController
  SharedMemory<DCMSensorData> naoSensorData;

  // NaoController --> DCM
  //SharedMemory<NaoCommandData> naoCommandData;
  SharedMemory<Accessor<MotorJointData> > naoCommandMotorJointData;
  SharedMemory<Accessor<UltraSoundSendData> > naoCommandUltraSoundSendData;
  SharedMemory<Accessor<LEDData> > naoCommandLEDData;

  // syncronize with NaoController
  sem_t* sem;
};

}

#endif //_LolaAdaptor_h_
