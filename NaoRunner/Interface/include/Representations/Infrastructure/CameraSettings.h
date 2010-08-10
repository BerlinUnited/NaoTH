/* 
 * File:   CameraSettings.h
 * Author: Oliver Welter
 *
 * Created on 15. März 2009, 16:56
 */

#ifndef _CAMERASETTINGS_H
#define	_CAMERASETTINGS_H

#include <string>

#include "Tools/Debug/ParameterList.h"

#include "Tools/DataStructures/Printable.h"
#include "PlatformInterface/PlatformInterchangeable.h"

using namespace std;



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
      Resolution,
      numOfCameraSetting
    };

    int data[numOfCameraSetting];

    CameraSettings();
    virtual ~CameraSettings();
    static string getCameraSettingsName(CameraSettingID id);

    virtual void print(ostream& stream) const;
};

class CurrentCameraSettings : public CameraSettings, public PlatformInterchangeable
{
public:
  CurrentCameraSettings();
  virtual ~CurrentCameraSettings() {};
};

class CameraSettingsRequest : public ParameterList,
  public CameraSettings, public PlatformInterchangeable
{
public:
  CameraSettingsRequest();
  virtual ~CameraSettingsRequest() {};

  /** Set to true to query a real CameraSetting from the Nao camera */
  bool queryCameraSettings;

};


#endif	/* _CAMERASETTINGS_H */

