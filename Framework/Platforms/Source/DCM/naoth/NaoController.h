/**
 * @file NaoController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the real robot for cognition
 *
 */

#ifndef _NAOCONTROLLER_H
#define	_NAOCONTROLLER_H

#include "SoundControl.h"
#include "V4lCameraHandler.h"
#include "TeamCommunicator.h"
#include "SPLGameController.h"
#include "Tools/NaoControllerBase.h"
#include "Representations/Infrastructure/TeamMessageData.h"
#include "Representations/Infrastructure/GameData.h"

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

  void get(TeamMessageDataIn& data);

  void get(GameData& data);
  
  void get(MotorJointData& data) { libNaothDataReading->get(data); }

  /////////////////////// set ///////////////////////
  void set(const CameraSettingsRequest& data);

  void set(const LEDData& data) { naothDataWriting->set(data); }

  void set(const IRSendData& data) { naothDataWriting->set(data); }

  void set(const UltraSoundSendData& data) { naothDataWriting->set(data); }

  void set(const SoundPlayData& data);

  void set(const TeamMessageDataOut& data);

  void set(const GameReturnData& data);
  
  virtual void getCognitionInput();
  
  virtual void setCognitionOutput();

private:
  V4lCameraHandler theCameraHandler;
  SoundControl *theSoundHandler;
  NaothData* naothDataWriting;
  TeamCommunicator* theTeamComm;
  SPLGameController* theGameController;
};

} // end namespace naoth
#endif	/* _NAOCONTROLLER_H */

