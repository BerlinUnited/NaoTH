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

/** Custom settings for V5 */
struct V5CameraSettings
{
  double targetGain;

  double minAnalogGain;
  double maxAnalogGain;

  bool fadeToBlack;

  int powerlineFrequency;

  int gammaCorrection;

  int autoExpositionAlgorithm;

  int whiteBalanceTemperature;
};

struct V6CameraSettings
{
  bool autoExposition;

  int exposure;
  int saturation;

  bool autoWhiteBalancing;
  int gain;
  int brightness;
  int contrast;
  int sharpness;
  int hue;
  
  int gain_red;
  int gain_green;
  int gain_blue;
  
  bool horizontalFlip;
  bool verticalFlip;
};

struct V5CommonSettings
{
  int whiteBalanceTemperature;
};

struct V6CommonSettings
{
  bool autoExposition;

  int exposure;
  int saturation;

  bool autoWhiteBalancing;
  int gain;
  int brightness;
  int contrast;
  int sharpness;
  int hue;
};

class CameraSettings : public Printable
{
public:
  bool autoExposition;

  int exposure;
  double gain;

  int saturation;

  int brightness;

  float contrast;

  int sharpness;

  int hue;

  bool autoWhiteBalancing;

  bool horizontalFlip;
  bool verticalFlip;

  bool backlightCompensation;

  V5CameraSettings v5;
  V6CameraSettings v6;

  static const std::size_t AUTOEXPOSURE_GRID_SIZE = 5;
  std::uint8_t autoExposureWeights[AUTOEXPOSURE_GRID_SIZE][AUTOEXPOSURE_GRID_SIZE];

  CameraSettings();
  virtual ~CameraSettings();

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

  V5CameraSettings v5;
  V6CameraSettings v6;
  bool autoWhiteBalancing;
  bool backlightCompensation;
  int brightness;
  int cameraSelection;
  double contrast;
  int exposure;
  double gain;
  bool horizontalFlip;
  int hue;
  int saturation;
  int sharpness;
  bool verticalFlip;

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

  int saturation;
  int sharpness;

  V5CommonSettings v5;
  V6CommonSettings v6;

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
} // namespace naoth

#endif /* _CAMERASETTINGS_H */
