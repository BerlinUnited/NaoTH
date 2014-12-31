/**
* @file CameraDebug.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class CameraDebug
*/

#ifndef _CameraDebug_h_
#define _CameraDebug_h_


#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Infrastructure/CameraSettings.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/Image.h"

//Debug
#include <DebugCommunication/DebugCommandManager.h>
#include <Tools/Debug/DebugRequest.h>
#include <Tools/Debug/DebugParameterList.h>

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(CameraDebug)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugCommandManager)

  REQUIRE(HeadMotionRequest)
  REQUIRE(CurrentCameraSettings)
  REQUIRE(CurrentCameraSettingsTop)
  REQUIRE(FrameInfo)

  PROVIDE(CameraSettingsRequest)
  PROVIDE(CameraSettingsRequestTop)
END_DECLARE_MODULE(CameraDebug)


//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class CameraDebug : private CameraDebugBase, public DebugCommandExecutor
{

public:
  CameraDebug();
  virtual ~CameraDebug();

  virtual void execute();

  virtual void executeDebugCommand(
    const std::string& command, 
    const std::map<std::string,std::string>& arguments,
    std::ostream& outstream);

private:
  bool isAutoCalibratingCamera;
  bool afterAutoCalibratingCamera;
  bool afterQueryCameraSettings;

  FrameInfo timeWhenCameraCalibrationStopped;
};

#endif //_CameraDebug_h_


