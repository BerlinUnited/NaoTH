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
    private:
    public:
      /* don't change order of enumerations
       * because it reflects the order of execution
       */
      enum CameraSettingID
      {
        CameraSelection,
        AutoExposition,
        AutoWhiteBalancing,
        AutoGain,
        Brightness,
        Contrast,
        Saturation,
        Hue,
        RedChroma,
        BlueChroma,
        Gain,
        HorizontalFlip,
        VerticalFlip,
        CorrectionLensX,
        CorrectionLensY,
        Exposure,
        ExposureCorrection,
        FPS,
        ResolutionWidth,
        ResolutionHeight,
        BacklightCompensation,
        WhiteBalance,
        Sharpness,
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
    CameraSettingsRequest();
    virtual ~CameraSettingsRequest() {}

    /** Set to true to query a real CameraSetting from the Nao camera */
    bool queryCameraSettings;

  };
}

#endif  /* _CAMERASETTINGS_H */

