/**
 * @file UltraSoundControl.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 * Handling the US sensors (mainly triggering)
 */

#ifndef _UltraSoundControl_h_
#define _UltraSoundControl_h_

#include <ModuleFramework/Module.h>

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/UltraSoundData.h"


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(UltraSoundControl)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)

  PROVIDE(UltraSoundSendData)
END_DECLARE_MODULE(UltraSoundControl)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class UltraSoundControl : private UltraSoundControlBase
{
public:
  UltraSoundControl();
  ~UltraSoundControl(){}

  virtual void execute();

private:  
  
  class Parameters: public ParameterList
  {
  public: 
    // FIXME the parameter is nut used in the cpp
    Parameters(): ParameterList("UltraSoundControl")
    {
      PARAMETER_REGISTER(receiveModeSendInterval) = 100;

      // load from the file after registering all parameters
      syncWithConfig();
    }

    int receiveModeSendInterval;
  } params;

private:
  // last time when US control mode was sent
  FrameInfo lastSend;
};

#endif //_UltraSoundControl_h_


