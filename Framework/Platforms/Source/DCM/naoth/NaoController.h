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
#include "Tools/Communication/Broadcast/BroadCaster.h"
#include "Tools/Communication/Broadcast/BroadCastListener.h"
#include "SPLGameController.h"
#include "Tools/NaoControllerBase.h"
#include "Representations/Infrastructure/TeamMessageData.h"
#include "Representations/Infrastructure/GameData.h"
#include "DebugCommunication/DebugServer.h"

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
  
  void get(MotorJointData& data) { /*libNaothDataReading->get(data);*/ }

  /////////////////////// set ///////////////////////
  void set(const CameraSettingsRequest& data);

  void set(const LEDData& data) { naoCommandDataWriting->set(data); }

  void set(const IRSendData& data) { naoCommandDataWriting->set(data); }

  void set(const UltraSoundSendData& data) { naoCommandDataWriting->set(data); }

  void set(const SoundPlayData& data);

  void set(const TeamMessageDataOut& data);

  void set(const GameReturnData& data);
  
  virtual void getCognitionInput();
  
  virtual void setCognitionOutput();


  

  void get(DebugMessageIn& data)
  {
    if(theDebugServer == NULL) return;
    theDebugServer->getDebugMessageIn(data);
  }

  void set(const DebugMessageOut& data)
  {
    if(theDebugServer == NULL) return;
    if(data.answers.size() > 0)
      theDebugServer->setDebugMessageOut(data);
  }

  void setDebugServer(DebugServer* server)
  {
    theDebugServer = server;
  }


private:
  V4lCameraHandler theCameraHandler;
  SoundControl *theSoundHandler;
  NaoCommandData* naoCommandDataWriting;
  BroadCaster* theBroadCaster;
  BroadCastListener* theBroadCastListener;
  SPLGameController* theGameController;

  DebugServer* theDebugServer;
};

} // end namespace naoth
#endif	/* _NAOCONTROLLER_H */

