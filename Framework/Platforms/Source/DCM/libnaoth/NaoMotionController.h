/**
 * @file NaoMotionController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the real robot for motion
 *
 */

#ifndef _NAO_MOTION_CONTROLLER_H
#define	_NAO_MOTION_CONTROLLER_H

#include "DCMHandler.h"
#include "Tools/NaoControllerBase.h"

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

  /////////////////////// init ///////////////////////
  void init(ALPtr<ALBroker> pB);
  
  virtual void getMotionInput();
  
  virtual void setMotionOutput();

public:

  /////////////////////// set ///////////////////////
  virtual void set(const MotorJointData& data);

private:
  DCMHandler theDCMHandler;
};

} // end namespace naoth
#endif	/* _NAO_MOTION_CONTROLLER_H */

