/**
* @file NaothModule.cpp
*
* @author <a href="mailto:welter@informatik.hu-berlin.de">Oliver Welter</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Deklaration of NaothModule
*/

#ifndef _NAOTHMODULE_H_
#define _NAOTHMODULE_H_

#include <alcommon/almodule.h>

#include "Tools/NaoTime.h"
#include "Tools/IPCData.h"
#include "Tools/BasicMotion.h"

#include "DCMHandler.h"

#define SMAL_VERSION "42"

namespace AL
{
  // This is a forward declaration of AL:ALBroker which
  // avoids including <alcommon/albroker.h> in this header
  class ALBroker;
}

namespace naoth
{

class SMALModule : public AL::ALModule
{
public:
    
  /**
  * Default Constructor.
  */
  SMALModule(boost::shared_ptr<AL::ALBroker> pBroker, const std::string& pName );

  /**
  * Destructor.
  */
  ~SMALModule();
  
  std::string version();
  
  //
  virtual void init();
  virtual void exit();

  //
  void motionCallbackPre();
  void motionCallbackPost();

  
private:
  // needed by theDCMHandler 
  boost::shared_ptr<AL::ALBroker> pBroker;

  // Used for sync with the DCM
  ProcessSignalConnection fDCMPreProcessConnection;
  ProcessSignalConnection fDCMPostProcessConnection;

  // interface for the interaction with the DCM
  DCMHandler theDCMHandler;
  
  
  //
  void setWarningLED();
  //
  bool runningEmergencyMotion();
  
  enum State
  {
    DISCONNECTED,
    CONNECTED
  } state;

  
  // Current dcm time, updated at each onPostProcess call.
  int dcmTime;
  // the offset between the system time and the dcm time
  int timeOffset;

  // DCM --> NaoController
  SharedMemory<NaoSensorData> naoSensorData;

  SharedMemory<int> debugSM;

  // NaoController --> DCM
  //SharedMemory<NaoCommandData> naoCommandData;
  SharedMemory<Accessor<MotorJointData> > naoCommandMotorJointData;
  SharedMemory<Accessor<UltraSoundSendData> > naoCommandUltraSoundSendData;
  SharedMemory<Accessor<IRSendData> > naoCommandIRSendData;
  SharedMemory<Accessor<LEDData> > naoCommandLEDData;

  // syncronize with NaoController
  sem_t* sem;


  // sitdown motion in case the Controller dies
  bool command_data_available;
  bool sensor_data_available;
  MotorJointData theMotorJointData;
  InertialSensorData theInertialSensorData;
  BasicMotion* initialMotion;
};

}//end namespace naoth

#endif	/* _NAOTHMODULE_H_ */
