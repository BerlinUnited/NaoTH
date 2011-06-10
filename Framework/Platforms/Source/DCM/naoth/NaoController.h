/**
 * @file NaoController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the real robot
 *
 */

#ifndef _NAOCONTROLLER_H
#define	_NAOCONTROLLER_H

#include "SoundControl.h"
#include "V4lCameraHandler.h"
#include "Tools/NaoControllerBase.h"
#include "Tools/SharedMemory.h"

namespace naoth
{
class NaoController : public NaoControllerBase<NaoController>
{
public:
  NaoController();
  virtual ~NaoController();

public:
  template<typename T>
  void get(T& data)
  {
    NaoControllerBase<NaoController>::get(data);
  }

  void get(Image& data);
  
  void get(CurrentCameraSettings& data);
  
  void get(MotorJointData& data) { libNaothDataReading->get(data); }

  /////////////////////// set ///////////////////////
  void set(const CameraSettingsRequest& data);

  void set(const LEDData& data) { naothDataWriting->set(data); }

  void set(const IRSendData& data) { naothDataWriting->set(data); }

  void set(const UltraSoundSendData& data) { naothDataWriting->set(data); }

  void set(const SoundPlayData& data);
  
  virtual void getCognitionInput();
  
  virtual void setCognitionOutput();

private:
  V4lCameraHandler theCameraHandler;
  SoundControl *theSoundHandler;
  
  SharedMemory<LibNaothData> libNaothData;
  
  SharedMemory<NaothData> naothData;
  NaothData* naothDataWriting;
};

} // end namespace naoth
#endif	/* _NAOCONTROLLER_H */

