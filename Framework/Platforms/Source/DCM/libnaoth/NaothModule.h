/**
* @file NaothModule.cpp
*
* @author <a href="mailto:welter@informatik.hu-berlin.de">Oliver Welter</a>
* Deklaration of NaothModule
*/

#ifndef _NAOTHMODULE_H
#define	_NAOTHMODULE_H

#include "Representations/Infrastructure/CameraInfo.h"
#include "Tools/NaoTime.h"
#include "Tools/IPCData.h"
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
  //
  ALPtr<ALBroker> pBroker;

  // Used for sync with the DCM
  ProcessSignalConnection fDCMPreProcessConnection;
  ProcessSignalConnection fDCMPostProcessConnection;

  // interface for the interaction with the DCM
  DCMHandler theDCMHandler;

  // DCM --> NaoController
  SharedMemory<NaoSensorData> naoSensorData;

  // NaoController --> DCM
  SharedMemory<NaoCommandData> naoCommandData;

  // syncronize with NaoController
  sem_t* sem;
};

} // end namespace naoth

#endif	/* _NAOTHMODULE_H */
