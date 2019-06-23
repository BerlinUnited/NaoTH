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

  bool autoExposition;

  int exposure;
  double gain;

  int saturation;

  bool autoWhiteBalancing;

  int whiteBalanceTemperature;

  bool horizontalFlip;
  bool verticalFlip;  

  // custom for V5
  float v5_targetGain;
  float v5_minAnalogGain;
  float v5_maxAnalogGain;

  /* don't change order of enumerations
       * because it reflects the order of execution
       */
  enum CameraSettingID
  {
    AutoExpositionAlgorithm,
    BacklightCompensation,
    Brightness,
    CameraSelection,
    Contrast,
    FadeToBlack,
    FPS, // TODO: remove this from settings?
    GammaCorrection,
    Hue,
    ResolutionHeight, // TODO: remove this from settings?
    ResolutionWidth,  // TODO: remove this from settings?
    Sharpness,
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

class CameraSettingsManager : public CameraSettings
{
public:
  
  /** Queries all values from the actual camera */
  virtual void query(int cameraFd , std::string cameraName, CameraSettings& settings ) = 0;

  /** Apply all changed values on the actual camera */
  virtual void apply(int cameraFd, std::string cameraName, const CameraSettings& settings ) = 0;
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
  int gammaCorrection;
  double v5_targetGain;
  double v5_minAnalogGain;
  double v5_maxAnalogGain;
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

  CameraSettings getCameraSettings(bool isV6 = false) const;
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
  int gammaCorrection;
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
} // namespace naoth

#endif /* _CAMERASETTINGS_H */
