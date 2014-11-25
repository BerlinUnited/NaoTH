/**
* @file SMALModule.h
*
* @author <a href="mailto:welter@informatik.hu-berlin.de">Oliver Welter</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of SMALModule
*/

#ifndef _SMALModule_H_
#define _SMALModule_H_

#include "Tools/IPCData.h"
#include "Tools/NaoTime.h"
#include "Tools/SharedMemoryIO.h"
#include "Tools/BasicMotion.h"

#include "DCMHandler.h"

#include <alcommon/almodule.h>

//
// This is to suppress the following gcc warning 
// thrown because by the old version of boost used by naoqi
// albroker.h and alproxy.h 
// produce those:
//   boost/function/function_base.hpp:325: 
//   warning: dereferencing type-punned pointer will break strict-aliasing rules
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <alcommon/alproxy.h>

#define SMAL_VERSION "42"

// NOTE: enables the low level debugging 
//#define DEBUG_SMAL

namespace naoth
{

class SMALModule : public AL::ALModule
{
public:
    
  SMALModule(boost::shared_ptr<AL::ALBroker> pBroker, const std::string& pName );
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
  
#ifdef DEBUG_SMAL
  // -- DEBUG: measure average execution times of the callbacks
  // motionCallbackPre and motionCallbackPost
  //
  // Number of runs 
  int counterPre;
  int counterPost;
  
  // last time the average time was calculated
  // measures the time of the call cycle
  long long lastPreTime;
  long long lastPostTime;
  
  // measure the time consumed by the functions body
  int time_motionCallbackPreSum;
  int time_motionCallbackPostSum;
  // -- DEBUG: END
#endif
 
  //
  void setWarningLED(bool red=false);
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
  bool shutdown_requested;
  MotorJointData theMotorJointData;
  InertialSensorData theInertialSensorData;
  ButtonData theButtonData;
  BasicMotion* initialMotion;
};

}//end namespace naoth

#endif	/* _SMALModule_H_ */
