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
    AutoExpositionAlgorithm,
    AutoWhiteBalancing,
    BacklightCompensation,
    Brightness,
    BrightnessDark,
    CameraSelection,
    Contrast,
    Exposure,
    FadeToBlack,
    FPS, // TODO: remove this from settings?
    Gain,
    MinAnalogGain,
    MaxAnalogGain,
    TargetGain,
    HorizontalFlip,
    Hue,
    ResolutionHeight, // TODO: remove this from settings?
    ResolutionWidth,  // TODO: remove this from settings?
    Saturation,
    Sharpness,
    VerticalFlip,
    WhiteBalance,
    PowerlineFrequency,
    numOfCameraSetting
  };

  int data[numOfCameraSetting];

  static const std::size_t AUTOEXPOSURE_GRID_SIZE = 5;
  std::uint8_t autoExposureWeights[AUTOEXPOSURE_GRID_SIZE][AUTOEXPOSURE_GRID_SIZE];

  CameraSettings();
  virtual ~CameraSettings();
  static std::string getCameraSettingsName(CameraSettingID id);

  virtual void print(std::ostream &stream) const;
};

class CurrentCameraSettings : public CameraSettings
{
public:
  CurrentCameraSettings();
  virtual ~CurrentCameraSettings() {}
};

class CurrentCameraSettingsTop : public CameraSettings
{
public:
  CurrentCameraSettingsTop();
  virtual ~CurrentCameraSettingsTop() {}
};

class CameraSettingsRequest : public ParameterList
{
public:
  CameraSettingsRequest(std::string configName = "CameraSettings");
  virtual ~CameraSettingsRequest() {}

  /** Set to true to query a real CameraSetting from the Nao camera */
  bool queryCameraSettings;

  bool autoExposition;
  int autoExpositionAlgorithm;
  bool autoWhiteBalancing;
  bool backlightCompensation;
  int brightness;
  int cameraSelection;
  double contrast;
  int exposure;
  bool fadeToBlack;
  double gain;
  double targetGain;
  double minAnalogGain;
  double maxAnalogGain;
  bool horizontalFlip;
  int hue;
  int saturation;
  int sharpness;
  bool verticalFlip;
  int whiteBalanceTemperature;
  int powerlineFrequency;

  std::uint8_t autoExposureWeights[CameraSettings::AUTOEXPOSURE_GRID_SIZE][CameraSettings::AUTOEXPOSURE_GRID_SIZE];

  void reset();
  void setAutoExposureWeights(std::uint8_t w);

  CameraSettings getCameraSettings() const;
};

class CameraSettingsRequestTop : public CameraSettingsRequest
{
public:
  CameraSettingsRequestTop();
  virtual ~CameraSettingsRequestTop() {}
};

//to be used to set camera setting commonly used for both cameras
class CommonCameraSettingsRequest : public ParameterList
{
public:
  CommonCameraSettingsRequest(std::string configName = "CommonCameraSettings");
  virtual ~CommonCameraSettingsRequest() {}

  bool autoExposition;
  bool autoWhiteBalancing;
  int brightness;
  int exposure;
  double gain;
  double targetGain;
  double minAnalogGain;
  double maxAnalogGain;

  int saturation;
  int sharpness;
  int whiteBalanceTemperature;
  int powerlineFrequency;

  /*
    Can be either:
      - averageY (average brightness over complete image)
      - dortmund (weights table like Dortmund uses)
      - centerlines3 (bottom 3 lines for top camera and top 3 lines for bottom camera)
      - centerlines2 (bottom 2 lines for top camera and top 2 lines for bottom camera)
    */
  std::string autoExpositionMethod;

  /** Set to true to query a real CameraSetting from the Nao camera */
  bool queryCameraSettings;

  bool isActive;
};
}

#endif /* _CAMERASETTINGS_H */
