/* 
 * File:   CameraSettings.h
 * Author: Oliver Welter
 *
 * Created on 15. march 2009, 16:56
 */

#ifndef _CameraSettings_H_
#define _CameraSettings_H_

#include <string>

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/ParameterList.h"

namespace naoth
{

  class CameraSettings : public Printable
  {
    public:
      /* don't change order of enumerations
       * because it reflects the order of execution
       */
      enum CameraSettingID
      {
        AutoExposition,
        AutoWhiteBalancing,
        BacklightCompensation,
        Brightness,
        CameraSelection,
        Contrast,
        Exposure,
        FadeToBlack,
        FPS, // TODO: remove this from settings?
        Gain,
        HorizontalFlip,
        Hue,
        ResolutionHeight, // TODO: remove this from settings?
        ResolutionWidth, // TODO: remove this from settings?
        Saturation,
        Sharpness,
        VerticalFlip,
        WhiteBalance,
        numOfCameraSetting
      };

      int data[numOfCameraSetting];

      CameraSettings();
      virtual ~CameraSettings();
      static std::string getCameraSettingsName(CameraSettingID id);

      virtual void print(std::ostream& stream) const;
  };

  class CurrentCameraSettings : public CameraSettings
  {
  public:
    CurrentCameraSettings();
    virtual ~CurrentCameraSettings() {}
  };

  class CameraSettingsRequest : public ParameterList, public CameraSettings
  {
  public:
    CameraSettingsRequest(std::string configName="CameraSettings");
    virtual ~CameraSettingsRequest() {}

    /** Set to true to query a real CameraSetting from the Nao camera */
    bool queryCameraSettings;

  };

  class CurrentCameraSettingsTop : public CameraSettings
  {
  public:
    CurrentCameraSettingsTop();
    virtual ~CurrentCameraSettingsTop() {}
  };

  class CameraSettingsRequestTop : public CameraSettingsRequest
  {
  public:
    CameraSettingsRequestTop();
    virtual ~CameraSettingsRequestTop() {}

  };
}

#endif  /* _CAMERASETTINGS_H */

