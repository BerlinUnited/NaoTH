/**
* @file NaothModule.cpp
*
* @author <a href="mailto:welter@informatik.hu-berlin.de">Oliver Welter</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Deklaration of NaothModule
*/

#ifndef _NAOTHMODULE_H_
#define _NAOTHMODULE_H_

#include "Tools/NaoTime.h"
#include "Tools/IPCData.h"
#include "Tools/BasicMotion.h"
#include "DCMHandler.h"

using namespace AL;
using namespace std;

namespace naoth
{

class NaothModule : public ALModule
{
public:
    
  /**
    * Default Constructor.
    */
  NaothModule(ALPtr<ALBroker> pBroker, const std::string& pName );
    
  /**
    * Destructor.
    */
  virtual ~NaothModule();
    
  /**
    * version
    * @return The version number of ALLeds
    */
  std::string version();
    
  bool innerTest();
    
  virtual void init();
  virtual void exit();

  void motionCallbackPre();
  void motionCallbackPost();

private:
  void setWarningLED();
  bool runningEmergencyMotion();

  enum State
  {
    DISCONNECTED,
    CONNECTED
  } state;

private:
  //
  ALPtr<ALBroker> pBroker;

  // Used for sync with the DCM
  ProcessSignalConnection fDCMPreProcessConnection;
  ProcessSignalConnection fDCMPostProcessConnection;

  // interface for the interaction with the DCM
  DCMHandler theDCMHandler;

  // Current dcm time, updated at each onPostProcess call.
  int dcmTime;
  // the offset between the system time and the dcm time
  int timeOffset;

  // DCM --> NaoController
  SharedMemory<NaoSensorData> naoSensorData;

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
  MotorJointData theMotorJointData;
  BasicMotion* initialMotion;
};

} // end namespace naoth

#endif	/* _NAOTHMODULE_H_ */
