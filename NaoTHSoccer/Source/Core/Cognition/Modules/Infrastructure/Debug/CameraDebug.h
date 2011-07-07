/**
* @file CameraDebug.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class CameraDebug
*/

#ifndef __CameraDebug_h_
#define __CameraDebug_h_


#include <ModuleFramework/Module.h>
#include <DebugCommunication/DebugCommandExecutor.h>

// Representations
#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Infrastructure/ColorTable64.h"
#include "Representations/Infrastructure/CameraSettings.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/Image.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(CameraDebug)
  REQUIRE(HeadMotionRequest)
  REQUIRE(CurrentCameraSettings)
  REQUIRE(FrameInfo)

  PROVIDE(ColorTable64)
  PROVIDE(CameraSettingsRequest)
  PROVIDE(FieldInfo)
  PROVIDE(Image)
END_DECLARE_MODULE(CameraDebug)


//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class CameraDebug : private CameraDebugBase, public DebugCommandExecutor
{

public:
  CameraDebug();
  ~CameraDebug(){};

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

#endif //__CameraDebug_h_


