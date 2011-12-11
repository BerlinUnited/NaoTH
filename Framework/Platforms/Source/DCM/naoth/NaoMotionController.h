/**
 * @file NaoMotionController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the real robot for motion
 *
 */

#ifndef _NAO_MOTION_CONTROLLER_H
#define	_NAO_MOTION_CONTROLLER_H

#include "Tools/NaoControllerBase.h"
#include "DebugCommunication/DebugServer.h"

namespace naoth
{
class NaoMotionController : public NaoControllerBase<NaoMotionController>
{
public:
  NaoMotionController();
  virtual ~NaoMotionController();

  template<typename T>
  void get(T& data)
  {
    NaoControllerBase<NaoMotionController>::get(data);
  }

  void set(const LEDData& data);



  virtual void getMotionInput();
  
  virtual void setMotionOutput();


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

public:

  /////////////////////// set ///////////////////////
  virtual void set(const MotorJointData& data);

private:
  //NaothData* naothDataWriting;

  DebugServer* theDebugServer;
};

} // end namespace naoth
#endif	/* _NAO_MOTION_CONTROLLER_H */

