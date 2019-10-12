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

#include "Representations/Infrastructure/FrameInfo.h"

//Debug
//#include <Tools/Debug/DebugRequest.h>
#include <Tools/Debug/DebugParameterList.h>

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(CameraDebug)
  //PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)

  REQUIRE(HeadMotionRequest)
  REQUIRE(CurrentCameraSettings)
  REQUIRE(CurrentCameraSettingsTop)
  REQUIRE(FrameInfo)

  PROVIDE(CameraSettingsRequest)
  PROVIDE(CameraSettingsRequestTop)

  PROVIDE(CommonCameraSettingsRequest)
END_DECLARE_MODULE(CameraDebug)


//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class CameraDebug : private CameraDebugBase
{

public:
  CameraDebug();
  virtual ~CameraDebug();

  virtual void execute();

private:
  template<class T>
  void setCommonCameraSetting(T value, T& bottomRequestValue, T& topRequestValue, bool& changed) {
    if(bottomRequestValue != value) {
      bottomRequestValue = value;
      changed = true;
    }
    if(topRequestValue != value) {
      topRequestValue = value;
      changed = true;
    }
  }

};

#endif //_CameraDebug_h_


